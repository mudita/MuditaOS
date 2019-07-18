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

//module-utils
#include "log/log.hpp"
//module-services
#include "service-evtmgr/EventManager.hpp"
#include "service-evtmgr/messages/EVMessages.hpp"
#include "service-appmgr/ApplicationManager.hpp"
//MessageType
#include "MessageType.hpp"
//this module
#include "windows/ClockMainWindow.hpp"
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
		sevm::KbdMessage* msg = static_cast<sevm::KbdMessage*>(msgl);
		LOG_INFO("Clock key received %d", static_cast<uint32_t>(msg->keyCode));

		if( msg->keyState == sevm::KeyboardEvents::keyReleasedShort ) {
			if( msg->keyCode == bsp::KeyCodes::JoystickLeft ) {
				sapm::ApplicationManager::messageSwitchApplication(this, "ApplicationViewer", "", nullptr );
			}

			if( msg->keyCode == bsp::KeyCodes::NumericKeyAst ) {

				auto it = windows.find("Main");
				gui::ClockMainWindow* win = reinterpret_cast<gui::ClockMainWindow*>( it->second );
				win->incrementHour();
				win->updateLabels();
			}
			if( msg->keyCode == bsp::KeyCodes::NumericKeyPnd ) {
				auto it = windows.find("Main");
				gui::ClockMainWindow* win = reinterpret_cast<gui::ClockMainWindow*>( it->second );
				win->incrementMinute();
				win->updateLabels();
			}
		}
		handled = true;
	}

	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked when timer ticked
void ApplicationClock::TickHandler(uint32_t id) {
	auto it = windows.find("Main");
	gui::ClockMainWindow* win = reinterpret_cast<gui::ClockMainWindow*>( it->second );
	win->incrementSecond();
	win->updateLabels();
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

	gui::ClockMainWindow* mainWindow = new gui::ClockMainWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(mainWindow->getName(), mainWindow));
}


void ApplicationClock::destroyUserInterface() {
}

} /* namespace app */
