/*
 * Service.cpp
 *
 *  Created on: Mar 7, 2019
 *      Author: mati
 */
#include "SystemManager.hpp"
#include "Service/LogOutput.hpp"

#include "thread.hpp"
#include "ticks.hpp"
#include "critical.hpp"

#include <algorithm>


namespace sys {

    using namespace cpp_freertos;
    using namespace std;
    using namespace sys;


    const char *systemManagerServiceName = "SysMgrService";

    SystemManager::SystemManager(TickType_t pingInterval)
            : Service(systemManagerServiceName),
              pingInterval(pingInterval) {
        // Specify list of channels which System Manager is registered to
        busChannels = {BusChannels::SystemManagerRequests};
    }

    SystemManager::~SystemManager() {
        LogOutput::Output(GetName() + ":destructor");
    }

    void SystemManager::Run() {

        InitHandler();

        if (userInit) {
            userInit();
        }

        while (enableRunLoop) {

            auto msg = mailbox.pop();

            msg->Execute(this);
        }
        Bus::Remove(shared_from_this());
        EndScheduler();
    }

    void SystemManager::StartSystem(std::function<int()> init) {
        LogOutput::Output("Initializing system...");

        userInit = init;

        lowPowerMode = bsp::LowPowerMode::Create().value_or(nullptr);

        // Start System manager
        StartService();

        //M.P: Ping/pong mechanism is turned off. It doesn't bring any value to the system.
        //pingPongTimerID = CreateTimer(Ticks::MsToTicks(pingInterval), true);
        //ReloadTimer(pingPongTimerID);

    }

    bool SystemManager::CloseSystem(Service *s) {

        Bus::SendUnicast(std::make_shared<SystemManagerMsg>(SystemManagerMsgType::CloseSystem),
                         systemManagerServiceName, s);
        return true;
    }

    bool SystemManager::SuspendSystem(Service *caller) {

        if(lowPowerMode->GetCurrentMode() != bsp::LowPowerMode::Mode::FullSpeed){
            return false;
        }

        for (auto w = servicesList.rbegin(); w != servicesList.rend(); ++w)
        {
            if((*w)->parent == "" && (*w)->GetName() != caller->GetName()){
                auto ret = Bus::SendUnicast(std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode,ServicePowerMode::SuspendToRAM),
                                            (*w)->GetName(), caller,1000);

                auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);
                if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)){
                    LOG_FATAL("Service %s failed to enter low-power mode",(*w)->GetName().c_str());
                }
            }
        }

        lowPowerMode->Switch(bsp::LowPowerMode::Mode::LowPowerIdle);
        return true; // TODO:M.P return real ret code

    }

    bool SystemManager::ResumeSystem(Service *caller) {

        if(lowPowerMode->GetCurrentMode() == bsp::LowPowerMode::Mode::FullSpeed){
            return false;
        }

        lowPowerMode->Switch(bsp::LowPowerMode::Mode::FullSpeed);

        for(const auto &w : servicesList){

            if(w->parent == "" && w->GetName() != caller->GetName()){
                auto ret = Bus::SendUnicast(std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode,ServicePowerMode::Active),
                                            w->GetName(), caller,1000);
                auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

                if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)){
                    LOG_FATAL("Service %s failed to exit low-power mode",w->GetName().c_str());
                }
            }
        }
        return true; // TODO:M.P return real ret code
    }

    bool SystemManager::SuspendService(const std::string &name, sys::Service *caller) {
        auto ret = Bus::SendUnicast(std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode,ServicePowerMode::SuspendToRAM),
                                    name, caller,1000);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)){
            LOG_FATAL("Service %s failed to exit low-power mode",name.c_str());
        }
        return true; // TODO:M.P return real ret code
    }

    bool SystemManager::ResumeService(const std::string &name, sys::Service *caller){
        auto ret = Bus::SendUnicast(std::make_shared<SystemMessage>(SystemMessageType::SwitchPowerMode,ServicePowerMode::Active),
                                    name, caller,1000);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first != ReturnCodes::Success && (resp->retCode != ReturnCodes::Success)){
            LOG_FATAL("Service %s failed to exit low-power mode",name.c_str());
        }
        return true; // TODO:M.P return real ret code
    }

    bool SystemManager::CreateService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout) {


        CriticalSection::Enter();
        servicesList.push_back(service);
        CriticalSection::Exit();

        service->StartService();

        auto msg = std::make_shared<SystemMessage>(SystemMessageType::Start);
        auto ret = Bus::SendUnicast(msg, service->GetName(), caller, timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)) {
            return true;
        } else {
            return false;
        }
    }

    bool SystemManager::DestroyService(const std::string &name, Service *caller, TickType_t timeout) {

        auto msg = std::make_shared<SystemMessage>(SystemMessageType::Exit);
        auto ret = Bus::SendUnicast(msg, name, caller, timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if (ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)) {

            cpp_freertos::LockGuard lck(destroyMutex);

            auto serv = std::find_if(servicesList.begin(), servicesList.end(),
                                     [&](std::shared_ptr<Service> const &s) { return s->GetName() == name; });
            if (serv == servicesList.end()) {
                return false;
            }

            servicesList.erase(serv);

            return true;
        } else {
            return false;
        }
    }


    ReturnCodes SystemManager::InitHandler() {
        isReady = true;
        return ReturnCodes::Success;
    }


    void SystemManager::TickHandler(uint32_t id) {
        if (id == pingPongTimerID) {

            for (auto &w : servicesList) {
                if (w->pingTimestamp == 0) {
                    //no reponse for ping messages, restart system
                    LogOutput::Output(w->GetName() + " failed to response to ping message");
                    exit(1);
                } else {
                    w->pingTimestamp = 0;
                }
            }

            Bus::SendBroadcast(std::make_shared<SystemMessage>(SystemMessageType::Ping), this);
        }

    }

    Message_t SystemManager::DataReceivedHandler(DataMessage *msg, ResponseMessage *resp) {
        if (msg->channel == BusChannels::SystemManagerRequests) {
            SystemManagerMsg *data = static_cast<SystemManagerMsg *>(msg);

            switch (data->type) {

                case SystemManagerMsgType::CloseSystem:
                    CloseSystemHandler();
                    break;

            }
        }


        return std::make_shared<ResponseMessage>();

    }

    void SystemManager::CloseSystemHandler() {
        LogOutput::Output("Invoking closing procedure...");

        DeleteTimer(pingPongTimerID);

        // We are going to remove services in reversed order of creation
        CriticalSection::Enter();
        std::reverse(servicesList.begin(), servicesList.end());
        CriticalSection::Exit();


        retry:
        for (auto &w : servicesList) {

            // Sysmgr stores list of all active services but some of them are under control of parent services.
            // Parent services ought to manage lifetime of child services hence we are sending DestroyRequests only to parents.
            if (w->parent == "") {
                auto ret = DestroyService(w->GetName(), this);
                if (!ret) {
                    //no response to exit message,
                    LogOutput::Output(w->GetName() + " failed to response to exit message");
                    exit(1);
                }
                goto retry;
            }
        }

        if (servicesList.size() == 0) {
            enableRunLoop = false;
        }

    }


    std::vector<std::shared_ptr<Service>> SystemManager::servicesList;
    cpp_freertos::MutexStandard SystemManager::destroyMutex;
    std::unique_ptr<bsp::LowPowerMode> SystemManager::lowPowerMode;

}
