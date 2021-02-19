// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManager.hpp"

#include "DependencyGraph.hpp"
#include "graph/TopologicalSort.hpp"

#include <common_data/EventStore.hpp>
#include "thread.hpp"
#include "ticks.hpp"
#include "critical.hpp"
#include <algorithm>
#include <service-evtmgr/KbdMessage.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-appmgr/messages/UserPowerDownRequest.hpp>
#include <Service/Timer.hpp>
#include <service-desktop/service-desktop/Constants.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/service-appmgr/Controller.hpp>
#include "messages/CpuFrequencyMessage.hpp"
#include "messages/DeviceRegistrationMessage.hpp"
#include "messages/SentinelRegistrationMessage.hpp"
#include "messages/RequestCpuFrequencyMessage.hpp"
#include "messages/PhoneModeRequest.hpp"
#include "messages/TetheringStateRequest.hpp"
#include <time/ScopedTime.hpp>

const inline size_t systemManagerStack = 4096 * 2;

namespace sys
{
    namespace
    {
        const std::map<bsp::KeyCodes, phone_modes::PhoneMode> SliderStateToPhoneModeMapping = {
            {bsp::KeyCodes::SSwitchUp, phone_modes::PhoneMode::Connected},
            {bsp::KeyCodes::SSwitchMid, phone_modes::PhoneMode::DoNotDisturb},
            {bsp::KeyCodes::SSwitchDown, phone_modes::PhoneMode::Offline}};

        constexpr auto preShutdownRoutineTimeout = 1500;
    } // namespace

    using namespace cpp_freertos;
    using namespace std;
    using namespace sys;

    void SystemManager::set(enum State state)
    {
        LOG_DEBUG("System manager state: [%s] -> [%s]", c_str(this->state), c_str(state));
        this->state = state;
    }

    SystemManager::SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators)
        : Service(service::name::system_manager, "", systemManagerStack), systemServiceCreators{std::move(creators)}
    {
        // Specify list of channels which System Manager is registered to
        bus.channels = {BusChannel::SystemManagerRequests};
    }

    SystemManager::~SystemManager()
    {
        LOG_DEBUG("%s", (GetName() + ":destructor").c_str());
    }

    void SystemManager::Run()
    {
        initialize();

        // in shutdown we need to wait till event manager tells us that it's ok to stfu
        while (state == State::Running) {
            if (auto msg = mailbox.pop(); msg) {
                msg->Execute(this);
            }
        }

        while (state == State::Shutdown) {
            // check if we are discharging - if so -> shutdown
            if (Store::Battery::get().state == Store::Battery::State::Discharging) {
                set(State::ShutdownReady);
            }
            else {
                // await from EvtManager for info that red key was pressed / timeout
                auto msg = mailbox.pop();
                if (!msg) {
                    continue;
                }
                if (msg->sender != service::name::evt_manager) {
                    LOG_ERROR("Ignored msg from: %s on shutdown", msg->sender.c_str());
                    continue;
                }
                msg->Execute(this);
            }
        }

        DestroySystemService(service::name::evt_manager, this);

        CloseService();

        EndScheduler();

        // Power off system
        switch (state) {
        case State::Reboot:
            LOG_INFO("  --->  REBOOT <--- ");
            powerManager->Reboot();
            break;
        case State::ShutdownReady:
            LOG_INFO("  ---> SHUTDOWN <--- ");
            powerManager->PowerOff();
            break;
        default:
            LOG_FATAL("State changed after reset/shutdown was requested to: %s! this is terrible failure!",
                      c_str(state));
            exit(1);
        };
    }

    void SystemManager::initialize()
    {
        utils::time::Scoped timer{"Initialize"};
        InitHandler();
        if (systemInit) {
            systemInit();
        }

        StartSystemServices();
        if (userInit) {
            userInit();
        }
    }

    void SystemManager::StartSystemServices()
    {
        DependencyGraph depGraph{graph::nodesFrom(systemServiceCreators), std::make_unique<graph::TopologicalSort>()};
        const auto &sortedServices = [&depGraph]() {
            utils::time::Scoped timer{"DependencyGraph"};
            return depGraph.sort();
        }();

        LOG_INFO("Order of system services initialization:");
        for (const auto &service : sortedServices) {
            LOG_INFO("\t> %s", service.get().getName().c_str());
        }
        std::for_each(sortedServices.begin(), sortedServices.end(), [this](const auto &service) {
            const auto startTimeout = service.get().getStartTimeout().count();
            if (const auto success = RunSystemService(service.get().create(), this, startTimeout); !success) {
                LOG_FATAL("Unable to start service: %s", service.get().getName().c_str());
            }
        });
    }

    void SystemManager::StartSystem(InitFunction sysInit, InitFunction appSpaceInit)
    {
        powerManager  = std::make_unique<PowerManager>();
        cpuStatistics = std::make_unique<CpuStatistics>();
        deviceManager = std::make_unique<DeviceManager>();
        phoneModeSubject = std::make_unique<phone_modes::Subject>(this);

        systemInit = std::move(sysInit);
        userInit   = std::move(appSpaceInit);

        // Start System manager
        StartService();

        cpuStatisticsTimer = std::make_unique<sys::Timer>("cpuStatistics", this, constants::timerInitInterval.count());
        cpuStatisticsTimer->connect([&](sys::Timer &) { CpuStatisticsTimerHandler(); });
        cpuStatisticsTimer->start();
    }

    bool SystemManager::Update(Service *s, const std::string &updateOSVer, std::string &currentOSVer)
    {
        // set update OS version (and also current os version) in Settings
        storeOsVersion(s, updateOSVer, currentOSVer);

        // close some services
        s->bus.sendUnicast(std::make_shared<SystemManagerCmd>(Code::Update), service::name::system_manager);

        // close some applications
        auto msgCloseApplications = std::make_shared<app::manager::UpdateInProgress>(service::name::system_manager);
        s->bus.sendUnicast(std::move(msgCloseApplications), app::manager::ApplicationManager::ServiceName);

        return true;
    }

    void SystemManager::storeOsVersion(Service *s, const std::string &updateOSVer, const std::string &currentOSVer)
    {
        // store OS version in Settings
        auto msgSetUpdateVersion = std::make_shared<app::manager::SetOsUpdateVersion>(
            service::name::system_manager, updateOSVer, currentOSVer);
        s->bus.sendUnicast(std::move(msgSetUpdateVersion), app::manager::ApplicationManager::ServiceName);
    }

    bool SystemManager::Reboot(Service *s)
    {
        s->bus.sendUnicast(std::make_shared<SystemManagerCmd>(Code::Reboot), service::name::system_manager);
        return true;
    }

    bool SystemManager::SuspendService(const std::string &name, sys::Service *caller)
    {
        auto ret = caller->bus.sendUnicast(
            std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode, ServicePowerMode::SuspendToRAM),
            name,
            1000);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)) {
            LOG_FATAL("Service %s failed to enter low-power mode", name.c_str());
        }
        return true;
    }

    bool SystemManager::ResumeService(const std::string &name, sys::Service *caller)
    {
        auto ret = caller->bus.sendUnicast(
            std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode, ServicePowerMode::Active), name, 1000);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)) {
            LOG_FATAL("Service %s failed to exit low-power mode", name.c_str());
        }
        return true;
    }

    bool SystemManager::RunService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout)
    {
        service->StartService();

        auto msg  = std::make_shared<SystemMessage>(SystemMessageType::Start);
        auto ret  = caller->bus.sendUnicast(msg, service->GetName(), timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)) {
            return true;
        }
        return false;
    }

    bool SystemManager::RunSystemService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout)
    {
        CriticalSection::Enter();
        servicesList.push_back(service);
        CriticalSection::Exit();

        return RunService(std::move(service), caller, timeout);
    }

    bool SystemManager::RunApplication(std::shared_ptr<Service> app, Service *caller, TickType_t timeout)
    {
        CriticalSection::Enter();
        applicationsList.push_back(app);
        CriticalSection::Exit();

        return RunService(std::move(app), caller, timeout);
    }

    bool SystemManager::DestroyService(std::vector<std::shared_ptr<Service>> &serviceContainer,
                                       const std::string &name,
                                       Service *caller,
                                       TickType_t timeout)
    {
        auto msg  = std::make_shared<SystemMessage>(SystemMessageType::Exit);
        auto ret  = caller->bus.sendUnicast(msg, name, timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)) {

            cpp_freertos::LockGuard lck(destroyMutex);

            auto serv = std::find_if(serviceContainer.begin(),
                                     serviceContainer.end(),
                                     [&name](std::shared_ptr<Service> const &s) { return s->GetName() == name; });
            if (serv == serviceContainer.end()) {
                LOG_ERROR("No such service to destroy in services list: %s", name.c_str());
                return false;
            }

            serviceContainer.erase(serv);

            return true;
        }
        else {
            LOG_ERROR("Service to close: %s doesn't exist", name.c_str());
            return false;
        }
    }

    bool SystemManager::DestroySystemService(const std::string &name, Service *caller, TickType_t timeout)
    {
        return DestroyService(servicesList, name, caller, timeout);
    }

    bool SystemManager::DestroyApplication(const std::string &name, Service *caller, TickType_t timeout)
    {
        return DestroyService(applicationsList, name, caller, timeout);
    }

    void SystemManager::preCloseRoutine(CloseReason closeReason)
    {
        for (const auto &service : servicesList) {
            auto msg = std::make_shared<ServiceCloseReasonMessage>(closeReason);
            bus.sendUnicast(std::move(msg), service->GetName());
            readyForCloseRegister.push_back(service->GetName());
        }

        servicesPreShutdownRoutineTimeout =
            std::make_unique<sys::Timer>("servicesPreShutdownRoutine", this, preShutdownRoutineTimeout);
        servicesPreShutdownRoutineTimeout->connect([&](sys::Timer &) { CloseServices(); });
        servicesPreShutdownRoutineTimeout->start();
    }

    void SystemManager::readyToCloseHandler(Message *msg)
    {
        if (!readyForCloseRegister.empty() && servicesPreShutdownRoutineTimeout->isCurrentlyActive()) {
            auto message = static_cast<ReadyToCloseMessage *>(msg);
            LOG_INFO("ready to close %s", message->sender.c_str());
            readyForCloseRegister.erase(
                std::remove(readyForCloseRegister.begin(), readyForCloseRegister.end(), message->sender),
                readyForCloseRegister.end());

            // All services responded
            if (readyForCloseRegister.empty()) {
                LOG_INFO("All services ready to close.");
                servicesPreShutdownRoutineTimeout->stop();
                CloseServices();
            }
        }
    }

    void SystemManager::kill(std::shared_ptr<Service> const &toKill)
    {
        auto ret = toKill->DeinitHandler();
        if (ret != sys::ReturnCodes::Success) {
            LOG_DEBUG("deinit handler: %s", c_str(ret));
        }
        toKill->CloseHandler();
    }

    ReturnCodes SystemManager::InitHandler()
    {
        isReady = true;

        connect(SystemManagerCmd(), [&](Message *msg) {
            if (msg->channel == BusChannel::SystemManagerRequests) {
                auto *data = static_cast<SystemManagerCmd *>(msg);

                switch (data->type) {
                case Code::CloseSystem:
                    CloseSystemHandler(data->closeReason);
                    break;
                case Code::Update:
                    UpdateSystemHandler();
                    break;
                case Code::Reboot:
                    RebootHandler();
                    break;
                case Code::None:
                    break;
                }
            }
            return MessageNone{};
        });

        connect(sevm::BatteryStatusChangeMessage(), [&](Message *) {
            if ((state == State::Shutdown) && (Store::Battery::get().state == Store::Battery::State::Discharging)) {
                set(State::ShutdownReady);
            }
            return MessageNone{};
        });

        connect(sevm::KbdMessage(), [&](Message *) {
            // we are in shutdown mode - we received that there was red key pressed -> we need to reboot
            if (state == State::Shutdown) {
                set(State::Reboot);
            }
            return MessageNone{};
        });

        connect(sevm::BatteryBrownoutMessage(), [&](Message *) {
            LOG_INFO("Battery Brownout voltage level reached! Closing system...");
            CloseSystemHandler(CloseReason::SystemBrownout);
            return MessageNone{};
        });

        connect(CellularCheckIfStartAllowedMessage(), [&](Message *) {
            EventManagerServiceAPI::checkBatteryLevelCriticalState(this);
            return MessageNone{};
        });

        connect(sevm::BatteryLevelCriticalMessage(), [&](Message *) {
            LOG_INFO("Battery Critical Level reached!");
            CellularServiceAPI::ChangeModulePowerState(this, cellular::State::PowerState::Off);

            auto msg = std::make_shared<CriticalBatteryLevelNotification>(true);
            bus.sendUnicast(msg, app::manager::ApplicationManager::ServiceName);

            return MessageNone{};
        });

        connect(sevm::BatteryLevelNormalMessage(), [&](Message *) {
            LOG_INFO("Battery level normal.");
            CellularServiceAPI::ChangeModulePowerState(this, cellular::State::PowerState::On);

            auto msg = std::make_shared<CriticalBatteryLevelNotification>(false);
            bus.sendUnicast(msg, app::manager::ApplicationManager::ServiceName);
            return MessageNone{};
        });

        connect(app::UserPowerDownRequest(), [&](Message *) {
            CloseSystemHandler(CloseReason::RegularPowerDown);
            return MessageNone{};
        });

        connect(ReadyToCloseMessage(), [&](Message *msg) {
            readyToCloseHandler(msg);
            return MessageNone{};
        });

        connect(typeid(sys::CpuFrequencyMessage), [this](sys::Message *message) -> sys::MessagePointer {
            auto msg = static_cast<sys::CpuFrequencyMessage *>(message);

            if (msg->getAction() == sys::CpuFrequencyMessage::Action::Increase) {
                powerManager->IncreaseCpuFrequency();
                cpuStatisticsTimer->reload();
            }
            else if (msg->getAction() == sys::CpuFrequencyMessage::Action::Decrease) {
                powerManager->DecreaseCpuFrequency();
                cpuStatisticsTimer->reload();
            }

            return sys::MessageNone{};
        });

        connect(typeid(sys::DeviceRegistrationMessage), [this](sys::Message *message) -> sys::MessagePointer {
            auto msg = static_cast<sys::DeviceRegistrationMessage *>(message);
            deviceManager->RegisterNewDevice(msg->getDevice());

            return sys::MessageNone{};
        });

        connect(typeid(sys::SentinelRegistrationMessage), [this](sys::Message *message) -> sys::MessagePointer {
            auto msg = static_cast<sys::SentinelRegistrationMessage *>(message);
            powerManager->RegisterNewSentinel(msg->getSentinel());

            return sys::MessageNone{};
        });

        connect(typeid(sys::RequestCpuFrequencyMessage), [this](sys::Message *message) -> sys::MessagePointer {
            auto msg = static_cast<sys::RequestCpuFrequencyMessage *>(message);
            powerManager->SetCpuFrequencyRequest(msg->getName(), msg->getRequest());

            return sys::MessageNone{};
        });

        connect(typeid(PhoneModeRequest), [this](sys::Message *message) -> sys::MessagePointer {
            auto request = static_cast<PhoneModeRequest *>(message);
            return handlePhoneModeRequest(request);
        });

        connect(typeid(TetheringStateRequest), [this](sys::Message *message) -> sys::MessagePointer {
            auto request = static_cast<TetheringStateRequest *>(message);
            return handleTetheringStateRequest(request);
        });

        deviceManager->RegisterNewDevice(powerManager->getExternalRamDevice());

        return ReturnCodes::Success;
    }

    MessagePointer SystemManager::DataReceivedHandler(DataMessage * /*msg*/, ResponseMessage * /*resp*/)
    {
        return std::make_shared<ResponseMessage>();
    }

    void SystemManager::CloseSystemHandler(CloseReason closeReason)
    {
        LOG_DEBUG("Invoking closing procedure...");

        // We are going to remove services in reversed order of creation
        CriticalSection::Enter();
        std::reverse(servicesList.begin(), servicesList.end());
        CriticalSection::Exit();

        preCloseRoutine(closeReason);
    }

    void SystemManager::CloseServices()
    {
        for (const auto &element : readyForCloseRegister) {
            LOG_INFO("Service: %s did not reported before timeout", element.c_str());
        }

        for (bool retry{};; retry = false) {
            for (auto &service : servicesList) {
                if (service->GetName() == service::name::evt_manager) {
                    LOG_DEBUG("Delay closing %s", service::name::evt_manager);
                    continue;
                }
                if (service->parent == "") {
                    const auto ret = DestroySystemService(service->GetName(), this);
                    if (!ret) {
                        // no response to exit message,
                        LOG_FATAL("%s", (service->GetName() + " failed to response to exit message").c_str());
                        kill(service);
                    }
                    retry = true;
                    break;
                }
            }
            if (!retry) {
                break;
            }
        }
        set(State::Shutdown);
    }

    void SystemManager::UpdateSystemHandler()
    {
        LOG_DEBUG("Starting system update procedure...");

        // We are going to remove services in reversed order of creation
        CriticalSection::Enter();
        std::reverse(servicesList.begin(), servicesList.end());
        CriticalSection::Exit();

        for (bool retry{};; retry = false) {
            for (auto &service : servicesList) {
                if (service->GetName() == service::name::evt_manager) {
                    LOG_DEBUG("Delay closing %s", service::name::evt_manager);
                    continue;
                }
                if (service->GetName() == service::name::service_desktop) {
                    LOG_DEBUG("Delay closing %s", service::name::service_desktop);
                    continue;
                }

                if (service->GetName() == service::name::db) {
                    LOG_DEBUG("Delay closing %s", service::name::db);
                    continue;
                }

                if (service->GetName() == service::name::gui) {
                    LOG_DEBUG("Delay closing %s", service::name::gui);
                    continue;
                }

                if (service->GetName() == service::name::eink) {
                    LOG_DEBUG("Delay closing %s", service::name::eink);
                    continue;
                }

                if (service->GetName() == app::manager::ApplicationManager::ServiceName) {
                    LOG_DEBUG("Delay closing %s", app::manager::ApplicationManager::ServiceName);
                    continue;
                }
                if (service->parent.empty()) {
                    const auto ret = DestroySystemService(service->GetName(), this);
                    if (!ret) {
                        // no response to exit message,
                        LOG_FATAL("%s failed to response to exit message", service->GetName().c_str());
                        kill(service);
                    }
                    retry = true;
                    break;
                }
            }
            if (!retry) {
                break;
            }
        }
    }

    void SystemManager::RebootHandler()
    {
        CloseSystemHandler(CloseReason::Reboot);
        set(State::Reboot);
    }

    void SystemManager::CpuStatisticsTimerHandler()
    {
        if (!cpuStatisticsTimerInit) {
            cpuStatisticsTimerInit = true;
            cpuStatisticsTimer->setInterval(constants::timerPeriodInterval.count());
        }

        cpuStatistics->Update();
        powerManager->UpdateCpuFrequency(cpuStatistics->GetPercentageCpuLoad());
    }

    phone_modes::PhoneMode SystemManager::translateSliderState(const RawKey &key)
    {
        const auto code = key.key_code;
        if (code != bsp::KeyCodes::SSwitchUp && code != bsp::KeyCodes::SSwitchMid &&
            code != bsp::KeyCodes::SSwitchDown) {
            throw std::invalid_argument{"Invalid key code passed."};
        }
        return SliderStateToPhoneModeMapping.at(code);
    }

    MessagePointer SystemManager::handlePhoneModeRequest(PhoneModeRequest *request)
    {
        LOG_INFO("Phone mode change requested.");
        phoneModeSubject->setPhoneMode(request->getPhoneMode());
        return MessageNone{};
    }

    MessagePointer SystemManager::handleTetheringStateRequest(TetheringStateRequest *request)
    {
        LOG_INFO("Tethering state change requested");
        phoneModeSubject->setTetheringMode(request->getTetheringState());
        return MessageNone{};
    }

    std::vector<std::shared_ptr<Service>> SystemManager::servicesList;
    std::vector<std::shared_ptr<Service>> SystemManager::applicationsList;
    cpp_freertos::MutexStandard SystemManager::destroyMutex;
    std::unique_ptr<PowerManager> SystemManager::powerManager;
    std::unique_ptr<CpuStatistics> SystemManager::cpuStatistics;
    std::unique_ptr<DeviceManager> SystemManager::deviceManager;

} // namespace sys
