/*
 * @file ApplicationCalendar.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "windows/CalendarMainWindow.hpp"
#include "ApplicationCalendar.hpp"

namespace app {

ApplicationCalendar::ApplicationCalendar(std::string name,uint32_t stackDepth,sys::ServicePriority priority) :
	Application( name, stackDepth, priority ) {
}

ApplicationCalendar::~ApplicationCalendar() {
}

// Invoked upon receiving data message
sys::Message_t ApplicationCalendar::DataReceivedHandler(sys::DataMessage* msgl) {
	return std::make_shared<sys::ResponseMessage>( );
}

    // Invoked when timer ticked
void ApplicationCalendar::TickHandler(uint32_t id) {
}

// Invoked during initialization
sys::ReturnCodes ApplicationCalendar::InitHandler() {

	createUserInterface();

	setActiveWindow("Main");

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationCalendar::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationCalendar::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ApplicationCalendar::SleepHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationCalendar::createUserInterface() {

	gui::Window* win = new CalendarMainWindow("Main");
	win->setSize( 480, 600 );

	windows.insert(std::pair<std::string,gui::Window*>(win->getName(), win));
}

void ApplicationCalendar::destroyUserInterface() {
}

} /* namespace app */
