
#include <memory>
#include "log/log.hpp"

#include "service-gui/ServiceGUI.hpp"
#include "service-eink/ServiceEink.hpp"
#include "service-kbd/ServiceKbd.hpp"

#include "bsp.hpp"
#include "vfs.hpp"
#include "keyboard/keyboard.hpp"

#include "SystemManager/SystemManager.hpp"

class vfs vfs;

class BlinkyService : public sys::Service {

public:
    BlinkyService(const std::string& name)
            : sys::Service(name)
    {
        timer_id = CreateTimer(1000,true);
        ReloadTimer(timer_id);
    }

    ~BlinkyService(){
    }

    // Invoked upon receiving data message
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
    //TODO:M.P remove it, only for test purposes
    bsp::keyboard keyboard;
    keyboard.Init([](bsp::KeyEvents event,bsp::KeyCodes code)->void{LOG_DEBUG("KeyEvent:%d KeyCode:%d",event,code);});


    vfs.Init();

    auto ret = sysmgr->CreateService(std::make_shared<BlinkyService>("BlinkyService"),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<ServiceEink>("ServiceEink"),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<ServiceGUI>("ServiceGUI"),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<ServiceKbd>("ServiceKbd"),sysmgr);

    if(ret){
        return 0;
    }

    return 0;
}


int main() {
    LOG_PRINTF("Launching PurePhone..\n");

    bsp::BoardInit();

    auto sysmgr = std::make_shared<sys::SystemManager>(5000);

    sysmgr->StartSystem();

    sysmgr->RegisterInitFunction(SystemStart);

    cpp_freertos::Thread::StartScheduler();

    return 0;
}
