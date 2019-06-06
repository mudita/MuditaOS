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

	LOG_DEBUG("ApplicationClock tick!");

	seconds++;
	if( seconds > 59 ) {
		minute++;
		seconds = 0;
	}

	if( minute > 59 ) {
		hour++;
		minute = 0;
	}
	if( hour > 23 ) {
		hour = 0;
	}

	std::string h;
	if( hour<10 ) {
		h += "0";
	}
	h += std::to_string(hour);
	std::string m;
	if( minute < 10 ) {
		m+= "0";
	}
	m += std::to_string(minute);

	std::string t = h+":"+m;
	UTF8 t8 = UTF8(t.c_str());
	timeLabel->setText( t8 );

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

	timeLabel = new gui::Label(clockWin, 100,225,280,150);
	timeLabel->setFilled( false );
	timeLabel->setBorderColor( gui::ColorNoColor );
	timeLabel->setFont("gt_pressura_bold_65");
	timeLabel->setText("12:35");
	timeLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
	timeLabel->setMaxSize( 480, 120 );

	windows.insert(std::pair<std::string,gui::Window*>(clockWin->getName(), clockWin));
}

void ApplicationClock::destroyUserInterface() {
}

} /* namespace app */
