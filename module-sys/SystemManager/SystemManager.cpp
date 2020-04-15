#include "SystemManager.hpp"

#include <common_data/EventStore.hpp>
#include "thread.hpp"
#include "ticks.hpp"
#include "critical.hpp"
#include <algorithm>
#include <service-evtmgr/messages/KbdMessage.hpp>
#include <service-evtmgr/messages/BatteryMessages.hpp>
#include <service-evtmgr/Constants.hpp>

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
        : Service(service::name::system_manager), pingInterval(pingInterval)
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

    void SystemManager::StartSystem(std::function<int()> init)
    {
        LOG_FATAL("Initializing system...");

        powerManager = std::make_unique<PowerManager>();
        // Switch system to full functionality(clocks and power domains configured to max values)
        powerManager->Switch(PowerManager::Mode::FullSpeed);
        userInit = init;

        // Start System manager
        StartService();

        // M.P: Ping/pong mechanism is turned off. It doesn't bring any value to the system.
        // pingPongTimerID = CreateTimer(Ticks::MsToTicks(pingInterval), true);
        // ReloadTimer(pingPongTimerID);
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

    bool SystemManager::SuspendSystem(Service *caller)
    {

        if (powerManager->GetCurrentMode() != PowerManager::Mode::FullSpeed) {
            LOG_WARN("System is already suspended.");
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

        powerManager->Switch(PowerManager::Mode::LowPowerIdle);

        return true;
    }

    bool SystemManager::ResumeSystem(Service *caller)
    {

        if (powerManager->GetCurrentMode() == PowerManager::Mode::FullSpeed) {
            LOG_WARN("System is already resumed.");
            return false;
        }

        powerManager->Switch(PowerManager::Mode::FullSpeed);

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
            LOG_FATAL("Service %s failed to exit low-power mode", name.c_str());
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

        connect(SystemManagerCmd(), [&](DataMessage *msg, ResponseMessage *resp) {
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

        connect(sevm::KbdMessage(), [&](DataMessage *msg, ResponseMessage *resp) {
            // we are in shutdown mode - we received that there was red key pressed -> we need to reboot
            if (state == State::Shutdown) {
                set(State::Reboot);
            }
            return Message_t();
        });

        connect(sevm::BatteryPlugMessage(), [&](DataMessage *msg, ResponseMessage *resp) {
            if (state == State::Shutdown) {
                set(State::ShutdownReady);
            }
            return Message_t();
        });

        return ReturnCodes::Success;
    }

    void SystemManager::TickHandler(uint32_t id)
    {
        if (id == pingPongTimerID) {

            for (auto &w : servicesList) {
                if (w->pingTimestamp == 0) {
                    // no reponse for ping messages, restart system
                    LOG_FATAL("%s", (w->GetName() + " failed to response to ping message").c_str());
                    exit(1);
                }
                else {
                    w->pingTimestamp = 0;
                }
            }

            Bus::SendBroadcast(std::make_shared<SystemMessage>(SystemMessageType::Ping), this);
        }
    }

    Message_t SystemManager::DataReceivedHandler(DataMessage *msg, ResponseMessage *resp)
    {
        return std::make_shared<ResponseMessage>();
    }

    void SystemManager::CloseSystemHandler()
    {
        LOG_DEBUG("Invoking closing procedure...");

        DeleteTimer(pingPongTimerID);

        // We are going to remove services in reversed order of creation
        CriticalSection::Enter();
        std::reverse(servicesList.begin(), servicesList.end());
        CriticalSection::Exit();

    retry:
        for (auto &service : servicesList) {
            if (service->GetName() == service::name::evt_manager) {
                LOG_DEBUG("Delay closing %s", service::name::evt_manager.c_str());
                continue;
            }
            if (service->parent == "") {
                auto ret = DestroyService(service->GetName(), this);
                if (!ret) {
                    // no response to exit message,
                    LOG_FATAL("%s", (service->GetName() + " failed to response to exit message").c_str());
                    kill(service);
                }
                goto retry;
            }
        }

        set(State::Shutdown);
    }

    void SystemManager::RebootHandler()
    {
        CloseSystemHandler();
        set(State::Reboot);
    }

    std::vector<std::shared_ptr<Service>> SystemManager::servicesList;
    cpp_freertos::MutexStandard SystemManager::destroyMutex;
    std::unique_ptr<PowerManager> SystemManager::powerManager;

} // namespace sys
