#include <iostream>
#include <memory>
#include "SystemManager/SystemManager.hpp"

#define LOG_MAIN
#define LOG_USE_COLOR
#include "log/log.h"


class BlinkyService : public core::Service {

public:
    BlinkyService(const std::string& name)
            : core::Service(name)
    {
        timer_id = CreateTimer("BlinkyTimer",1000,true);
        ReloadTimer(timer_id);
    }

    ~BlinkyService(){
    }

    // Invoked when service received data message
    void DataReceivedHandler(core::DataMessage* msgl) override{
    }

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override{
        LOG_DEBUG("Tick handler!");
    }

    // Invoked during initialization
    void InitHandler() override{

    }

    void DeinitHandler() override{
    }

    void WakeUpHandler() override{
    }


    void SleepHandler() override{
    }

    uint32_t timer_id= 0;


};


int main() {

    auto sysmgr = std::make_shared<core::SystemManager>(20000,5000);

    sysmgr->StartSystem();

    sysmgr->CreateService(std::make_shared<BlinkyService>("BlinkyService"),0);

    cpp_freertos::Thread::StartScheduler();

    return 0;
}