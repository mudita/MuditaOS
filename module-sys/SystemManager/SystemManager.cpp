/*
 * Service.cpp
 *
 *  Created on: Mar 7, 2019
 *      Author: mati
 */
#include "SystemManager.hpp"

#include "thread.hpp"
#include "ticks.hpp"
#include "critical.hpp"
#include <algorithm>

namespace sys
{

    using namespace cpp_freertos;
    using namespace std;
    using namespace sys;

    const char *systemManagerServiceName = "SysMgrService";

    SystemManager::SystemManager(TickType_t pingInterval)
        : Service(systemManagerServiceName, "", 8192), pingInterval(pingInterval)
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
        while (state == State::Running || state == State::Shutdown) {
            auto msg = mailbox.pop();
            msg->Execute(this);
        }

        DestroyService(service::name::evt_manager, this);
        while (state == State::Reboot && PreShutdownLoop()) {
            vTaskDelay(100);
        }

        Bus::Remove(shared_from_this());
        EndScheduler();

        // Power off system
        switch (state) {
        case State::Reboot:
            powerManager->Reboot();
            break;
        case State::Shutdown:
            powerManager->PowerOff();
            break;
        default:
            LOG_FATAL("State changed after reset/shutdown was requested! this is terrible failure!");
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
        Bus::SendUnicast(
            std::make_shared<SystemManagerMsg>(SystemManagerMsgType::CloseSystem), systemManagerServiceName, s);
        return true;
    }

    bool SystemManager::Reboot(Service *s)
    {
        Bus::SendUnicast(std::make_shared<SystemManagerMsg>(SystemManagerMsgType::Reboot), systemManagerServiceName, s);
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
        // needs merging -> LOG_DEBUG("deinit handler: %s", c_str(ret));
        toKill->CloseHandler();
    }

    ReturnCodes SystemManager::InitHandler()
    {
        isReady = true;
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
        if (msg->channel == BusChannels::SystemManagerRequests) {
            auto *data = static_cast<SystemManagerMsg *>(msg);

            switch (data->type) {
            case SystemManagerMsgType::CloseSystem:
                CloseSystemHandler();
                break;
            case SystemManagerMsgType::Reboot:
                RebootHandler();
                break;
            }
        }
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
                    LOG_FATAL("%s", service->GetName() + " failed to response to exit message");
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
