// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManager.hpp"

#include <common_data/EventStore.hpp>
#include "thread.hpp"
#include "ticks.hpp"
#include "critical.hpp"
#include <algorithm>
#include <service-evtmgr/messages/KbdMessage.hpp>
#include <service-evtmgr/messages/BatteryMessages.hpp>
#include <service-evtmgr/Constants.hpp>
#include "messages/ScreenStateMessage.hpp"

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

    SystemManager::SystemManager(TickType_t pingInterval)
        : Service(service::name::system_manager, "", systemManagerStack), pingInterval(pingInterval)
    {
        // Specify list of channels which System Manager is registered to
        busChannels = {BusChannels::SystemManagerRequests};
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
            auto msg = mailbox.pop();
            msg->Execute(this);
        }

        while (state == State::Shutdown) {
            // check if we are discharging - if so -> shutdown
            if (Store::Battery::get().state == Store::Battery::State::Discharging) {
                set(State::ShutdownReady);
            }
            else {
                // await from EvtManager for info that red key was pressed / timeout
                auto msg = mailbox.pop();
                if (msg->sender != service::name::evt_manager) {
                    LOG_ERROR("Ignored msg from: %s on shutdown", msg->sender.c_str());
                    continue;
                }
                msg->Execute(this);
            }
        }

        DestroyService(service::name::evt_manager, this);

        Bus::Remove(shared_from_this());
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
        LOG_FATAL("Initializing system...");

        powerManager = std::make_unique<PowerManager>();
        // Switch system to full functionality(clocks and power domains configured to max values)
        powerManager->Switch(PowerManager::Mode::FullSpeed);
        userInit = init;

        systemState = std::make_unique<SystemState>();

        // Start System manager
        StartService();

        // M.P: Ping/pong mechanism is turned off. It doesn't bring any value to the system.
        // pingPongTimerID = CreateTimer(Ticks::MsToTicks(pingInterval), true);
        // ReloadTimer(pingPongTimerID);

        lowPowerEntryTimer =
            std::make_unique<sys::Timer>("LowPowerEnryTimer", this, LongWait, sys::Timer::Type::SingleShot);
        lowPowerEntryTimer->connect([&](sys::Timer &) { LowPowerEntryHandler(); });
    }

    bool SystemManager::CloseSystem(Service *s)
    {
        Bus::SendUnicast(std::make_shared<SystemManagerCmd>(Code::CloseSystem), service::name::system_manager, s);
        return true;
    }

    bool SystemManager::Reboot(Service *s)
    {
        Bus::SendUnicast(std::make_shared<SystemManagerCmd>(Code::Reboot), service::name::system_manager, s);
        return true;
    }

    bool SystemManager::FocusOnApplication(Service *caller, bool focusEnabled)
    {
        systemState->SetFocusOnApplication(focusEnabled ? SystemState::ApplicationState::Active
                                                        : SystemState::ApplicationState::Unactive);
        if (focusEnabled) {
            return ExitLowPowerMode(caller);
        }
        lowPowerEntryTimer->reload(ShortWait);
        return true;
    }

    bool SystemManager::UpdateChargerState(Service *caller, bool plugged)
    {
        systemState->SetChargerState(plugged ? SystemState::ChargerState::Plugged
                                             : SystemState::ChargerState::Unplugged);
        if (plugged) {
            return ExitLowPowerMode(caller);
        }
        lowPowerEntryTimer->reload(ShortWait);
        return true;
    }

    bool SystemManager::UpdateCellularState(Service *caller, bool ready)
    {
        systemState->SetCellularState(ready ? SystemState::CellularState::Ready : SystemState::CellularState::Unready);
        if (ready) {
            lowPowerEntryTimer->reload(ShortWait);
        }
        else {
            return ExitLowPowerMode(caller);
        }
        return true;
    }

    bool SystemManager::UpdateAudioState(Service *caller, bool active)
    {
        systemState->SetAudioState(active ? SystemState::AudioState::Active : SystemState::AudioState::Unactive);
        if (active) {
            return ExitLowPowerMode(caller);
        }
        lowPowerEntryTimer->reload(ShortWait);
        return true;
    }

    void SystemManager::RtcUpdate()
    {
        systemState->SetRtcState(SystemState::RtcState::Updated);
        lowPowerEntryTimer->reload(ShortWait);
    }

    void SystemManager::PrepareSystemToHandleCellular(Service *caller)
    {
        lowPowerEntryTimer->reload(LongWait);
        ExitLowPowerMode(caller);
    }

    void SystemManager::CheckLowPowerMode(Service *caller)
    {

        lowPowerEntryTimer->reload(ShortWait);
        ExitLowPowerMode(caller);
    }

    bool SystemManager::EnterLowPowerMode(Service *caller)
    {
        if (powerManager->GetCurrentMode() != PowerManager::Mode::FullSpeed) {
            return false;
        }

        if (!systemState->LowPowerPermission()) {
            return false;
        }

        for (auto w = servicesList.rbegin(); w != servicesList.rend(); ++w) {
            if ((*w)->parent == "" && (*w)->GetName() != caller->GetName()) {
                auto ret = Bus::SendUnicast(
                    std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode, ServicePowerMode::SuspendToRAM),
                    (*w)->GetName(),
                    caller,
                    1000);

                auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);
                if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)) {
                    LOG_FATAL("Service %s failed to enter low-power mode", (*w)->GetName().c_str());
                }
            }
        }

        CriticalSection::Enter();
        powerManager->Switch(PowerManager::Mode::LowPowerIdle);
        CriticalSection::Exit();

        return true;
    }

    bool SystemManager::ExitLowPowerMode(Service *caller)
    {
        if (powerManager->GetCurrentMode() == PowerManager::Mode::FullSpeed) {
            return false;
        }

        CriticalSection::Enter();
        powerManager->Switch(PowerManager::Mode::FullSpeed);
        CriticalSection::Exit();

        for (const auto &w : servicesList) {
            if (w->parent == "" && w->GetName() != caller->GetName()) {
                auto ret = Bus::SendUnicast(
                    std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode, ServicePowerMode::Active),
                    w->GetName(),
                    caller,
                    1000);
                auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

                if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)) {
                    LOG_FATAL("Service %s failed to exit low-power mode", w->GetName().c_str());
                }
            }
        }

        return true;
    }

    bool SystemManager::SuspendService(const std::string &name, sys::Service *caller)
    {
        auto ret = Bus::SendUnicast(
            std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode, ServicePowerMode::SuspendToRAM),
            name,
            caller,
            1000);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)) {
            LOG_FATAL("Service %s failed to enter low-power mode", name.c_str());
        }
        return true;
    }

    bool SystemManager::ResumeService(const std::string &name, sys::Service *caller)
    {
        auto ret = Bus::SendUnicast(
            std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode, ServicePowerMode::Active),
            name,
            caller,
            1000);
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
        auto ret  = Bus::SendUnicast(msg, service->GetName(), caller, timeout);
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
        auto ret  = Bus::SendUnicast(msg, name, caller, timeout);
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

        connect(SystemManagerCmd(), [&](DataMessage *msg, ResponseMessage * /*resp*/) {
            if (msg->channel == BusChannels::SystemManagerRequests) {
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
            return Message_t();
        });

        connect(sevm::KbdMessage(), [&](DataMessage * /*msg*/, ResponseMessage * /*resp*/) {
            // we are in shutdown mode - we received that there was red key pressed -> we need to reboot
            if (state == State::Shutdown) {
                set(State::Reboot);
            }
            return Message_t();
        });

        connect(sevm::BatteryPlugMessage(), [&](DataMessage * /*msg*/, ResponseMessage * /*resp*/) {
            if (state == State::Shutdown) {
                set(State::ShutdownReady);
            }
            return Message_t();
        });

        connect(typeid(sys::ScreenStateMessage),
                [this](sys::DataMessage *message, sys::ResponseMessage *) -> sys::Message_t {
                    auto msg                             = static_cast<sys::ScreenStateMessage *>(message);
                    sys::ScreenStateMessage::State state = msg->getState();
                    if (state == sys::ScreenStateMessage::State::Lock) {
                        systemState->SetScreenState(SystemState::ScreenState::Lock);
                        lowPowerEntryTimer->reload(ShortWait);
                    }
                    else {
                        systemState->SetScreenState(SystemState::ScreenState::Unlock);
                    }
                    return Message_t();
                });

        return ReturnCodes::Success;
    }

    Message_t SystemManager::DataReceivedHandler(DataMessage * /*msg*/, ResponseMessage * /*resp*/)
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
            if (!retry)
                break;
        }
        set(State::Shutdown);
    }

    void SystemManager::RebootHandler()
    {
        CloseSystemHandler();
        set(State::Reboot);
    }

    void SystemManager::LowPowerEntryHandler()
    {
        /// To disable entering LowPower mode, comment out the line below
        EnterLowPowerMode(this);
    }

    std::unique_ptr<sys::Timer> SystemManager::lowPowerEntryTimer;
    std::vector<std::shared_ptr<Service>> SystemManager::servicesList;
    cpp_freertos::MutexStandard SystemManager::destroyMutex;
    std::unique_ptr<PowerManager> SystemManager::powerManager;
    std::unique_ptr<SystemState> SystemManager::systemState;

} // namespace sys
