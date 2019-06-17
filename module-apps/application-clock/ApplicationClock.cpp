/*
 * @file ApplicationClock.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

//module-gui
#include "gui/widgets/Window.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/core/ImageManager.hpp"
//module-utils
#include "log/log.hpp"
//module-services
#include "service-evtmgr/EventManager.hpp"
#include "service-appmgr/ApplicationManager.hpp"
//MessageType
#include "MessageType.hpp"
//this module
#include "ApplicationClock.hpp"

namespace app {

ApplicationClock::ApplicationClock(std::string name,uint32_t stackDepth,sys::ServicePriority priority) :
	Application( name, stackDepth, priority ) {

	timer_id = CreateTimer(100,true);
	ReloadTimer(timer_id);

}

ApplicationClock::~ApplicationClock() {
	// TODO Auto-generated destructor stub
}

// Invoked upon receiving data message
sys::Message_t ApplicationClock::DataReceivedHandler(sys::DataMessage* msgl) {

	auto retMsg = Application::DataReceivedHandler(msgl);
	//if message was handled by application's template there is no need to process further.
	if( (reinterpret_cast<sys::ResponseMessage*>( retMsg.get() )->retCode ==
		sys::ReturnCodes::Success ) ){
		return retMsg;
	}

	//this variable defines whether message was processed.
	bool handled = false;
	//if keyboard message received
	if(msgl->messageType == static_cast<uint32_t>(MessageType::KBDKeyEvent) )
	{
		KbdMessage* msg = static_cast<KbdMessage*>(msgl);
		LOG_INFO("Clock key received %d", static_cast<uint32_t>(msg->keyCode));

		if( msg->keyState == KeyboardEvents::keyReleasedShort ) {
			if( msg->keyCode == bsp::KeyCodes::JoystickLeft ) {
				sapm::ApplicationManager::messageSwitchApplication(this, "ApplicationViewer", "" );
			}
			if( msg->keyCode == bsp::KeyCodes::NumericKeyAst ) {
				incrementHour();
				updateLabels();
			}
			if( msg->keyCode == bsp::KeyCodes::NumericKeyPnd ) {
				incrementMinute();
				updateLabels();
			}
		}
		handled = true;
	}

	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

void ApplicationClock::updateLabels() {
	std::string h;
	if( hour<10 ) {
		h += "0";
	}
	h += std::to_string(hour);
	hourLabel->setText( h );

	std::string m;
	if( minute < 10 ) {
		m+= "0";
	}
	m += std::to_string(minute);
	minuteLabel->setText( m );
}

    // Invoked when timer ticked
void ApplicationClock::TickHandler(uint32_t id) {

	incrementSecond();
	updateLabels();
	render(gui::RefreshModes::GUI_REFRESH_FAST );
}

// Invoked during initialization
sys::ReturnCodes ApplicationClock::InitHandler() {

	auto ret = Application::InitHandler();
	if( ret != sys::ReturnCodes::Success )
		return ret;

	createUserInterface();

	setActiveWindow("Main");

	return ret;
}

sys::ReturnCodes ApplicationClock::DeinitHandler() {
	DeleteTimer( timer_id );
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationClock::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ApplicationClock::SleepHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationClock::createUserInterface() {

	gui::Window* clockWin = new gui::Window("Main");
	clockWin->setSize( 480, 600 );

	uint32_t xOffset = 0;
	uint32_t yOffset = 0;

	hourLabel = new gui::Label(clockWin, 100+xOffset,300-160+yOffset,280,150);
	hourLabel->setFilled( false );
	hourLabel->setBorderColor( gui::ColorNoColor );
	hourLabel->setFont("gt_pressura_regular_140");
	hourLabel->setText("00");
	hourLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

	minuteLabel = new gui::Label(clockWin, 100+xOffset,310+yOffset,280,150);
	minuteLabel->setFilled( false );
	minuteLabel->setBorderColor( gui::ColorFullBlack );
	minuteLabel->setFont("gt_pressura_regular_140");
	minuteLabel->setText("00");
	minuteLabel->setRadius( 20 );
	minuteLabel->setPenWidth(1);
	minuteLabel->setFillColor( gui::Color(10,0));
	minuteLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

	progressBar = new gui::Progress(clockWin, 480/2-90+xOffset, 300-6+yOffset, 180, 12 );
	progressBar->setTotalProgress(59);
	progressBar->setCurrentProgress(0);

	windows.insert(std::pair<std::string,gui::Window*>(clockWin->getName(), clockWin));
}

bool ApplicationClock::incrementHour() {
	hour++;
	if( hour > 23 ) {
		hour = 0;
		return true;
	}
	return false;
}
bool ApplicationClock::incrementMinute() {
	minute++;
	if( minute > 59 ) {
		incrementHour();
		minute = 0;
		return true;
	}
	return false;
}

bool ApplicationClock::incrementSecond(){
	bool ret = false;
	seconds++;
	if( seconds > 59 ) {
		incrementMinute();
		seconds = 0;
		ret = true;
	}
	progressBar->setCurrentProgress(seconds);

	return ret;
}

void ApplicationClock::destroyUserInterface() {
}

} /* namespace app */
