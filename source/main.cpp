
#include <memory>
#include <list>

#include "../module-gui/gui/core/ImageManager.hpp"
#include "log/log.hpp"
#include "memory/usermem.h"
#include "ticks.hpp"

//module-applications
#include  "application-clock/ApplicationClock.hpp"

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
public:
    BlinkyService(const std::string& name)
            : sys::Service(name)
    {
        timer_id = CreateTimer(1000,true);
        ReloadTimer(timer_id);



/*        win = new gui::Window("Main");
		win->setSize( 480, 600 );

		gui::HBox* hBox = new gui::HBox( win, 50, 50, 380, 500 );

		gui::Rect* maxW1 = new gui::Rect();
		maxW1->setFillColor(gui::Color( 5, 0));
		maxW1->setFilled(true);
		maxW1->setMaxSize( 50, 300 );

		gui::Label* maxW4 = new gui::Label();
		maxW4->setText("Top Left corner");
		maxW4->setDotsMode(true);
		maxW4->setMaxSize( 275, 60 );

		gui::Rect* maxW2 = new gui::Rect();
		maxW2->setFillColor(gui::Color( 8, 0));
		maxW2->setFilled(true);
		maxW2->setMaxSize( 35, 300 );

		gui::Rect* maxW3 = new gui::Rect();
		maxW3->setFillColor(gui::Color( 11, 0));
		maxW3->setFilled(true);
		maxW3->setMaxSize( 30, 300 );

		hBox->addWidget(maxW1);
		hBox->addWidget(maxW4);
		hBox->addWidget(maxW2);

		gui::VBox* vBox = new gui::VBox( hBox, 10, 155, 460, 600-160 );

		gui::Rect* maxH1 = new gui::Rect();
		maxH1->setMaxSize( 10, 80 );

		gui::Rect* maxH2 = new gui::Rect();
		maxH2->setMaxSize( 15, 300 );

		gui::Rect* maxH3 = new gui::Rect();
		maxH3->setMaxSize( 30, 300 );

		gui::Label* maxH4 = new gui::Label();
		maxH4->setText("Hello Mudita");
		maxH4->setRadius( 20 );
		maxH4->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
		maxH4->setMaxSize( 75, 60 );

		gui::Image* img1 = new gui::Image();
		uint16_t id = gui::ImageManager::getInstance().getImageMapID("loudspeaker.mpi");
		img1->setImageWithID( id );


		vBox->addWidget(maxH1);
		vBox->addWidget(maxH2);
		vBox->addWidget(maxH4);
		vBox->addWidget( img1 );
		vBox->addWidget(maxH3);

		hBox->addWidget(maxW3);*/
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
        LOG_DEBUG("Blinky service tick!");
        uint32_t timestamp1 = cpp_freertos::Ticks::GetTicks();
        auto ret = DBServiceAPI::SettingsGet(this);
        LOG_ERROR("SettingsGet perf: %lu",cpp_freertos::Ticks::GetTicks()-timestamp1);

        ret.language = SettingsLanguage ::GERMAN;

        DBServiceAPI::SettingsUpdate(this,ret);

        LOG_ERROR("Available heap: %lu",usermemGetFreeHeapSize());
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
   // bsp::keyboard keyboard;
   // keyboard.Init([](bsp::KeyEvents event,bsp::KeyCodes code)->void{LOG_DEBUG("KeyEvent:%d KeyCode:%d",event,code);});


    vfs.Init();

    auto ret = sysmgr->CreateService(std::make_shared<sgui::ServiceGUI>("ServiceGUI", 480, 600 ),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<ServiceEink>("ServiceEink"),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<EventManager>("EventManager"),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<ServiceDB>(),sysmgr);
    ret |= sysmgr->CreateService(std::make_shared<BlinkyService>("Blinky"),sysmgr);

    sysmgr->DestroyService(ServiceDB::serviceName,sysmgr);

    //vector with launchers to applications
    std::vector< std::unique_ptr<app::ApplicationLauncher> > applications;

    //launcher for clock application
    std::unique_ptr<app::ApplicationLauncher> clockLauncher = std::unique_ptr<app::ApplicationClockLauncher>(new app::ApplicationClockLauncher());
    applications.push_back( std::move(clockLauncher) );

    //start application manager
    ret |= sysmgr->CreateService(std::make_shared<sapm::ApplicationManager>("ApplicationManager",sysmgr,applications),sysmgr );

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
