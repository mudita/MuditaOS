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
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
//module-utils
#include "log/log.hpp"
//this module
#include "ApplicationClock.hpp"

namespace app {

ApplicationClock::ApplicationClock(std::string name,uint32_t stackDepth,sys::ServicePriority priority) :
	Application( name, stackDepth, priority ) {

	timer_id = CreateTimer(1000,true);
	ReloadTimer(timer_id);

}

ApplicationClock::~ApplicationClock() {
	// TODO Auto-generated destructor stub
}

// Invoked upon receiving data message
sys::Message_t ApplicationClock::DataReceivedHandler(sys::DataMessage* msgl) {
	return std::make_shared<sys::ResponseMessage>( );
}

    // Invoked when timer ticked
void ApplicationClock::TickHandler(uint32_t id) {

	bool res = incrementSecond();

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

	if( res )
		render(gui::RefreshModes::GUI_REFRESH_FAST );

}

// Invoked during initialization
sys::ReturnCodes ApplicationClock::InitHandler() {

	createUserInterface();

	setActiveWindow("Main");

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationClock::DeinitHandler() {
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

	hourLabel = new gui::Label(clockWin, 100,300-160,280,150);
	hourLabel->setFilled( false );
	hourLabel->setBorderColor( gui::ColorNoColor );
	hourLabel->setFont("gt_pressura_regular_140");
	hourLabel->setText("12");
	hourLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

	minuteLabel = new gui::Label(clockWin, 100,310,280,150);
	minuteLabel->setFilled( false );
	minuteLabel->setBorderColor( gui::ColorNoColor );
	minuteLabel->setFont("gt_pressura_regular_140");
	minuteLabel->setText("05");
	minuteLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

	gui::Rect* rect = new gui::Rect( clockWin, 480/2-30, 300-4, 60, 8 );
	rect->setFillColor( gui::ColorFullBlack );
	rect->setFilled( true );

	gui::Rect* rectCircle = new gui::Rect( clockWin, 10, 70, 460, 460 );
	rectCircle->setRadius(230);
	rectCircle->setFilled( false );


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
	seconds++;
	if( seconds > 59 ) {
		incrementMinute();
		seconds = 0;
		return true;
	}
	return false;
}

void ApplicationClock::destroyUserInterface() {
}

} /* namespace app */
