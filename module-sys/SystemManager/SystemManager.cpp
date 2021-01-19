// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManager.hpp"

#include <common_data/EventStore.hpp>
#include "thread.hpp"
#include "ticks.hpp"
#include "critical.hpp"
#include <algorithm>
#include <service-evtmgr/KbdMessage.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <Service/Timer.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>
#include "messages/CpuFrequencyMessage.hpp"
#include "messages/DeviceRegistrationMessage.hpp"

const inline size_t systemManagerStack = 4096 * 2;

namespace sys
{

    using namespace cpp_freertos;
    using namespace std;
    using namespace sys;

    void SystemManager::set(enum State state)
    {
        LOG_DEBUG("System manager state: [%s] -> [%s]", c_str(this->state), c_str(state));
        this->state = state;
    }

    SystemManager::SystemManager() : Service(service::name::system_manager, "", systemManagerStack)
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

        InitHandler();

        if (userInit) {
            userInit();
        }

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

        DestroyService(service::name::evt_manager, this);

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

    void SystemManager::StartSystem(InitFunction init)
    {
        powerManager  = std::make_unique<PowerManager>();
        cpuStatistics = std::make_unique<CpuStatistics>();
        deviceManager = std::make_unique<DeviceManager>();

        userInit = init;

        // Start System manager
        StartService();

        cpuStatisticsTimer = std::make_unique<sys::Timer>("cpuStatistics", this, constants::timerInitInterval.count());
        cpuStatisticsTimer->connect([&](sys::Timer &) { CpuStatisticsTimerHandler(); });
        cpuStatisticsTimer->start();
    }

    bool SystemManager::CloseSystem(Service *s)
    {
        s->bus.sendUnicast(std::make_shared<SystemManagerCmd>(Code::CloseSystem), service::name::system_manager);
        return true;
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

    bool SystemManager::CreateService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout)
    {

        CriticalSection::Enter();
        servicesList.push_back(service);
        CriticalSection::Exit();

        service->StartService();

        auto msg  = std::make_shared<SystemMessage>(SystemMessageType::Start);
        auto ret  = caller->bus.sendUnicast(msg, service->GetName(), timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)) {
            return true;
        }
        else {
            return false;
        }
    }

    bool SystemManager::DestroyService(const std::string &name, Service *caller, TickType_t timeout)
    {

        auto msg  = std::make_shared<SystemMessage>(SystemMessageType::Exit);
        auto ret  = caller->bus.sendUnicast(msg, name, timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)) {

            cpp_freertos::LockGuard lck(destroyMutex);

            auto serv = std::find_if(servicesList.begin(), servicesList.end(), [&](std::shared_ptr<Service> const &s) {
                return s->GetName() == name;
            });
            if (serv == servicesList.end()) {
                LOG_ERROR("No such service to destroy in services list: %s", name.c_str());
                return false;
            }

            servicesList.erase(serv);

            return true;
        }
        else {
            LOG_ERROR("Service to close: %s doesn't exist", name.c_str());
            return false;
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
                    CloseSystemHandler();
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

        connect(sevm::KbdMessage(), [&](Message *) {
            // we are in shutdown mode - we received that there was red key pressed -> we need to reboot
            if (state == State::Shutdown) {
                set(State::Reboot);
            }
            return MessageNone{};
        });

        connect(sevm::BatteryPlugMessage(), [&](Message *) {
            if (state == State::Shutdown) {
                set(State::ShutdownReady);
            }
            return MessageNone{};
        });

        connect(sevm::BatteryBrownoutMessage(), [&](Message *) {
            LOG_INFO("Battery Brownout voltage level reached!");

            auto msg = std::make_shared<SystemBrownoutMesssage>();
            bus.sendUnicast(msg, app::manager::ApplicationManager::ServiceName);

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

        deviceManager->RegisterNewDevice(powerManager->getExternalRamDevice());

        return ReturnCodes::Success;
    }

    MessagePointer SystemManager::DataReceivedHandler(DataMessage * /*msg*/, ResponseMessage * /*resp*/)
    {
        return std::make_shared<ResponseMessage>();
    }

    void SystemManager::CloseSystemHandler()
    {
        LOG_DEBUG("Invoking closing procedure...");

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
                if (service->parent == "") {
                    const auto ret = DestroyService(service->GetName(), this);
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

    void SystemManager::RebootHandler()
    {
        CloseSystemHandler();
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

    std::vector<std::shared_ptr<Service>> SystemManager::servicesList;
    cpp_freertos::MutexStandard SystemManager::destroyMutex;
    std::unique_ptr<PowerManager> SystemManager::powerManager;
    std::unique_ptr<CpuStatistics> SystemManager::cpuStatistics;
    std::unique_ptr<DeviceManager> SystemManager::deviceManager;

} // namespace sys
