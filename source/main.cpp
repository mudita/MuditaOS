#include <memory>
#include "SystemManager/SystemManager.hpp"
#include "log/log.hpp"
#include "gui/widgets/Window.hpp"


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

class GuiTestService : public sys::Service {

	gui::Window* win = new gui::Window( 0 );

public:
	GuiTestService(const std::string& name)
            : sys::Service(name)
    {
        timer_id = CreateTimer(1000,true);
        ReloadTimer(timer_id);
    }

    ~GuiTestService(){
    	if( win )
    		delete win;
    }

    // Invoked upon receiving data message
    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override{
        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override{
        LOG_DEBUG("GuiTestService service tick!");
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
    ret |= sysmgr->CreateService(std::make_shared<GuiTestService>("GuiTestService"),sysmgr);

    if(ret){
        return 0;
    }

    return 0;
}


int main() {

    auto sysmgr = std::make_shared<sys::SystemManager>(5000);

    sysmgr->StartSystem();

    sysmgr->RegisterInitFunction(SystemStart);

    cpp_freertos::Thread::StartScheduler();

    return 0;
}
