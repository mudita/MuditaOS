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


namespace sys
{

    using namespace cpp_freertos;
    using namespace std;
    using namespace sys;


    const char* systemManagerServiceName = "SysMgrService";

    SystemManager::SystemManager(TickType_t pingInterval)
            : Service(systemManagerServiceName,4096,ServicePriority::Idle),
              pingInterval(pingInterval)
    {
        // Specify list of channels which System Manager is registered to
        busChannels = {BusChannels ::SystemManagerRequests};
    }

    SystemManager::~SystemManager()
    {
        LogOutput::Output(GetName() + ":destructor");
    }

    void SystemManager::Run(){

        InitHandler();

        if(userInit){
            userInit();
        }

        while (enableRunLoop) {

            auto msg = mailbox.pop();

            msg->Execute(this);
        }
        Bus::Remove(shared_from_this());
        EndScheduler();
    }

    void SystemManager::StartSystem(std::function<int()> init)
    {
        LogOutput::Output("Initializing system...");

        userInit = init;

        // Start System manager
        StartService();

        pingPongTimerID = CreateTimer(Ticks::MsToTicks(pingInterval),true);
        ReloadTimer(pingPongTimerID);

    }

    bool SystemManager::CloseSystem(Service* s){

        Bus::SendUnicast(std::make_shared<SystemManagerMsg>(SystemManagerMsgType::CloseSystem),systemManagerServiceName,s);
        return true;
    }

    bool SystemManager::CreateService(std::shared_ptr<Service> service,Service* caller,TickType_t timeout){


        CriticalSection::Enter();
        servicesList.push_back(service);
        CriticalSection::Exit();

        service->StartService();

        auto msg = std::make_shared<SystemMessage>(SystemMessageType::Start);
        auto ret = Bus::SendUnicast(msg,service->GetName(),caller,timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if(ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)){
            return true;
        }
        else{
            return false;
        }
    }

    bool SystemManager::DestroyService(const std::string& name,Service* caller,TickType_t timeout){

        auto msg = std::make_shared<SystemMessage>(SystemMessageType::Exit);
        auto ret = Bus::SendUnicast(msg,name,caller,timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if(ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)){

            cpp_freertos::LockGuard lck(destroyMutex);

            auto serv = std::find_if(servicesList.begin(),servicesList.end(),[&](std::shared_ptr<Service>const &s) { return s->GetName() == name; });
            if(serv == servicesList.end()){
                return false;
            }

            servicesList.erase(serv);

            return true;
        }
        else{
            return false;
        }
    }

    bool SystemManager::SuspendService(const std::string& name,Service* caller,TickType_t timeout){

        auto msg = std::make_shared<SystemMessage>(SystemMessageType::GoSleep);
        auto ret = Bus::SendUnicast(msg,name,caller,timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if(ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)){
            return true;
        }
        else{
            return false;
        }
    }

    bool SystemManager::ResumeService(const std::string& name,Service* caller,TickType_t timeout){

        auto msg = std::make_shared<SystemMessage>(SystemMessageType::Wakeup);
        auto ret = Bus::SendUnicast(msg,name,caller,timeout);
        auto resp = std::static_pointer_cast<ResponseMessage>(ret.second);

        if(ret.first == ReturnCodes::Success && (resp->retCode == ReturnCodes::Success)){
            return true;
        }
        else{
            return false;
        }
    }

    ReturnCodes SystemManager::InitHandler(){
        isReady = true;
        return ReturnCodes ::Success;
    }


    void SystemManager::TickHandler(uint32_t id)
    {
        if(id == pingPongTimerID){

            for(auto &w : servicesList){
                if(w->pingTimestamp==0){
                    //no reponse for ping messages, restart system
                    LogOutput::Output(w->GetName() + " failed to response to ping message");
                    exit(1);
                }
                else{
                    w->pingTimestamp = 0;
                }
            }

            Bus::SendBroadcast(std::make_shared<SystemMessage>(SystemMessageType::Ping),this);
        }

    }

    Message_t SystemManager::DataReceivedHandler(DataMessage* msg,ResponseMessage* resp)
    {
        if(msg->channel == BusChannels::SystemManagerRequests){
            SystemManagerMsg* data = static_cast<SystemManagerMsg*>(msg);

            switch(data->type){

                case SystemManagerMsgType::CloseSystem:
                    CloseSystemHandler();
                    break;

            }
        }


        return std::make_shared<ResponseMessage>();

    }

    void SystemManager:: CloseSystemHandler(){
        LogOutput::Output("Invoking closing procedure...");

        DeleteTimer(pingPongTimerID);

        // We are going to remove services in reversed order of creation
        CriticalSection::Enter();
        std::reverse(servicesList.begin(),servicesList.end());
        CriticalSection::Exit();

        servicesList.erase( std::remove_if(servicesList.begin(), servicesList.end(), [&](auto& obj){
            // Sysmgr stores list of all active services but some of them are under control of parent services.
            // Parent services ought to manage lifetime of child services hence we are sending close messages only to parent services.
            if(obj->parent == ""){
                auto ret = Bus::SendUnicast(std::make_shared<SystemMessage>(SystemMessageType::Exit),obj->GetName(),this,5000);

                if(ret.first != ReturnCodes::Success){
                    //no response to exit message,
                    LogOutput::Output(obj->GetName() + " failed to response to exit message");
                    exit(1);
                }
            }
            return true;
        }
        ), servicesList.end() );
        
        if(servicesList.size() == 0){
                enableRunLoop = false;
        }

    }


    std::vector<std::shared_ptr<Service>> SystemManager::servicesList;
    cpp_freertos::MutexStandard SystemManager::destroyMutex;

}
