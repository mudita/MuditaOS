#include <memory>
#include "SystemManager/SystemManager.hpp"
#include "log/log.hpp"


class BlinkyService : public sys::Service {

public:
    BlinkyService(const std::string& name)
            : sys::Service(name)
    {
        timer_id = CreateTimer("BlinkyTimer",1000,true);
        ReloadTimer(timer_id);
    }

    ~BlinkyService(){
    }

    // Invoked when service received data message
    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override{
        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override{
        LOG_DEBUG("Blinky service tick!");
    }

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override{
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes DeinitHandler() override{
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes WakeUpHandler() override{
        return sys::ReturnCodes::Success;
    }


    sys::ReturnCodes SleepHandler() override{
        return sys::ReturnCodes::Success;
    }

    uint32_t timer_id= 0;


};

int SystemStart(sys::SystemManager* sysmgr)
{
    auto ret = sysmgr->CreateService(std::make_shared<BlinkyService>("BlinkyService"),sysmgr);

    if(ret){
        return 0;
    }

    return 0;
}


int main() {

    //auto sysmgr = std::make_shared<sys::SystemManager>(5000);

    //sysmgr->StartSystem();

    //sysmgr->RegisterInitFunction(SystemStart);

    cpp_freertos::Thread::StartScheduler();

    return 0;
}