
#include <memory>
#include <list>

#include "../module-gui/gui/core/ImageManager.hpp"
#include "log/log.hpp"
#include "memory/usermem.h"
#include "ticks.hpp"

//module-applications
#include  "application-clock/ApplicationClock.hpp"
#include  "application-viewer/ApplicationViewer.hpp"
#include "application-test/ApplicationTest.hpp"

//module-services
#include "service-gui/ServiceGUI.hpp"
#include "service-gui/messages/DrawMessage.hpp"
#include "ServiceEink.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-kbd/EventManager.hpp"


#include "service-db/ServiceDB.hpp"
#include "service-db/api/DBServiceAPI.hpp"

//module-gui
#include "gui/core/Font.hpp"
#include "gui/core/BoundingBox.hpp"
#include "gui/core/Context.hpp"
#include "gui/core/Renderer.hpp"
#include "gui/core/DrawCommand.hpp"
#include "gui/core/Font.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/widgets/Image.hpp"

//module-bsp
#include "bsp.hpp"
#include "vfs.hpp"
#include "keyboard/keyboard.hpp"

#include "SystemManager/SystemManager.hpp"

class vfs vfs;

class BlinkyService : public sys::Service {
	gui::Window* win = nullptr;
	uint8_t* mem = nullptr;
public:
    BlinkyService(const std::string& name)
            : sys::Service(name)
    {
    	mem = new uint8_t[480*600];
        timer_id = CreateTimer(100,true);
        ReloadTimer(timer_id);

    }

    ~BlinkyService(){
    	if( win )
    		delete win;
    }

    // Invoked upon receiving data message
    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override{

        return std::make_shared<sys::ResponseMessage>( );
    }

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override{
        //auto msg = std::make_shared<sys::DataMessage>(500);
        //sys::Bus::SendUnicast(msg,"Blinky",this);
        //LOG_DEBUG("Blinky service tick!");
    	uint32_t start_tick = xTaskGetTickCount();
		memset( mem, 0, 480*600);
		uint32_t end_tick = xTaskGetTickCount();
		//LOG_DEBUG("memset time: %d", end_tick-start_tick);
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
    vfs.Init();

    bool ret;
    ret = sysmgr->CreateService(std::make_shared<sgui::ServiceGUI>("ServiceGUI", 480, 600 ),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<ServiceEink>("ServiceEink"),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<EventManager>("EventManager"),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<ServiceDB>(),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<BlinkyService>("Blinky"),sysmgr);

    //vector with launchers to applications
    std::vector< std::unique_ptr<app::ApplicationLauncher> > applications;

#if 1 // TODO: Robert please clean it up
    //launcher for clock application
    std::unique_ptr<app::ApplicationLauncher> clockLauncher = std::unique_ptr<app::ApplicationClockLauncher>(new app::ApplicationClockLauncher());
    applications.push_back( std::move(clockLauncher) );

//    //launcher for viewer application
//	std::unique_ptr<app::ApplicationLauncher> viewerLauncher = std::unique_ptr<app::ApplicationViewerLauncher>(new app::ApplicationViewerLauncher());
//	applications.push_back( std::move(viewerLauncher) );
#endif

    //start application manager
   ret |= sysmgr->CreateService(std::make_shared<sapm::ApplicationManager>("ApplicationManager",sysmgr,applications),sysmgr );

    if(ret){
        return 0;
    }

    return 0;
}

int main() {

	LOG_PRINTF("Launching PurePhone..\n ");

    bsp::BoardInit();

    auto sysmgr = std::make_shared<sys::SystemManager>(5000);

    sysmgr->StartSystem();

    sysmgr->RegisterInitFunction(SystemStart);



    cpp_freertos::Thread::StartScheduler();

    return 0;
}
