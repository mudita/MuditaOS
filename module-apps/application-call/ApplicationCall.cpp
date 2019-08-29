/*
 * @file ApplicationCall.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "Application.hpp"

#include "MessageType.hpp"
#include "windows/CallMainWindow.hpp"
#include "windows/EnterNumberWindow.hpp"
#include "windows/EmergencyCallWindow.hpp"
#include "windows/CallWindow.hpp"
#include "data/CallSwitchData.hpp"

#include "service-cellular/ServiceCellular.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "service-audio/api/AudioServiceAPI.hpp"

#include "service-appmgr/ApplicationManager.hpp"

#include "ApplicationCall.hpp"
namespace app {

ApplicationCall::ApplicationCall(std::string name, std::string parent, bool startBackgound ) :
	Application( name, parent, startBackgound, 4096 ) {

	timerCall = CreateTimer(1000,true);
}

ApplicationCall::~ApplicationCall() {
}

// Invoked upon receiving data message
sys::Message_t ApplicationCall::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

	auto retMsg = Application::DataReceivedHandler(msgl);
	//if message was handled by application's template there is no need to process further.
	if( (reinterpret_cast<sys::ResponseMessage*>( retMsg.get() )->retCode ==
		sys::ReturnCodes::Success ) ){
		return retMsg;
	}

	//this variable defines whether message was processed.
	bool handled = false;
	if( msgl->messageType == static_cast<int32_t>(MessageType::CellularNotification) ) {
			CellularNotificationMessage *msg = reinterpret_cast<CellularNotificationMessage *>(msgl);

		gui::CallWindow* callWindow = reinterpret_cast<gui::CallWindow*>( windows.find( "CallWindow")->second);

		if (msg->type == CellularNotificationMessage::Type::CallAborted) {
		   LOG_INFO("---------------------------------CallAborted");
		   AudioServiceAPI::Stop(this);
		   callEndTime = callDuration + 3;
		   callWindow->setState( gui::CallWindow::State::CALL_ENDED );
		   refreshWindow( gui::RefreshModes::GUI_REFRESH_DEEP );
		}
		else if( msg->type == CellularNotificationMessage::Type::CallBusy) {
			callEndTime = callDuration + 3;
		    LOG_INFO("---------------------------------CallBusy");
		    AudioServiceAPI::Stop(this);
		    callWindow->setState( gui::CallWindow::State::CALL_ENDED );
		    refreshWindow( gui::RefreshModes::GUI_REFRESH_DEEP );
		}
		else if( msg->type == CellularNotificationMessage::Type::CallActive ) {
		   LOG_INFO("---------------------------------CallActive");
		   callWindow->setState( gui::CallWindow::State::CALL_IN_PROGRESS );
		   refreshWindow( gui::RefreshModes::GUI_REFRESH_DEEP );
		}
		else if( msg->type == CellularNotificationMessage::Type::IncomingCall ) {
			//reset call duration
		    callDuration = 0;
			LOG_INFO("---------------------------------IncomingCall");
			if( callWindow->getState() == gui::CallWindow::State::INCOMING_CALL ) {
				LOG_INFO("ignoring call incoming");
			}
			AudioServiceAPI::RoutingStart(this);
			runCallTimer();
			std::unique_ptr<gui::SwitchData> data = std::make_unique<app::IncommingCallData>(msg->data);
			//send to itself message to switch (run) call application
			if( state == State::ACTIVE_FORGROUND ) {
				switchWindow( "CallWindow",0, std::move(data) );
			}
			else {
				callWindow->setState( gui::CallWindow::State::INCOMING_CALL );
				sapm::ApplicationManager::messageSwitchApplication( this, "ApplicationCall", "CallWindow", std::move(data) );
			}
		}
		else if( msg->type == CellularNotificationMessage::Type::Ringing ) {
			//reset call duration
		    callDuration = 0;
			LOG_INFO("---------------------------------Ringing");
			AudioServiceAPI::RoutingStart(this);
			runCallTimer();
			callWindow->setState( gui::CallWindow::State::OUTGOING_CALL );
			if( state == State::ACTIVE_FORGROUND ) {
				switchWindow( "CallWindow",0, nullptr );
			}
		}
		handled = true;
	}

	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ApplicationCall::InitHandler() {

	auto ret = Application::InitHandler();
	if( ret != sys::ReturnCodes::Success )
		return ret;

	createUserInterface();

	setActiveWindow("MainWindow");

	return ret;
}

sys::ReturnCodes ApplicationCall::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

// Invoked when timer ticked, 3 seconds after end call event if user didn't press back button earlier.
void ApplicationCall::TickHandler(uint32_t id) {
	++callDuration;

	auto it = windows.find("CallWindow");
	if( currentWindow == it->second ) {
		gui::CallWindow* callWindow = reinterpret_cast<gui::CallWindow*>(currentWindow);

		if( callWindow->getState() == gui::CallWindow::State::CALL_IN_PROGRESS ) {
			callWindow->updateDuration( callDuration );
			refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
		}
	}

	if( callDuration >= callEndTime ) {
		stopTimer(timerCall);
		sapm::ApplicationManager::messageSwitchPreviousApplication( this );
	}
}

void ApplicationCall::stopCallTimer() {
	LOG_INFO("switching to precv calldur: %d endTime: %d", callDuration, callEndTime );
	stopTimer(timerCall);
	sapm::ApplicationManager::messageSwitchPreviousApplication( this );
}

void ApplicationCall::createUserInterface() {

	gui::AppWindow* window = nullptr;

	window = new gui::CallMainWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::EnterNumberWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::CallWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::EmergencyCallWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));
}

void ApplicationCall::setDisplayedNumber( std::string num ) {
	phoneNumber = num;
}

const std::string& ApplicationCall::getDisplayedNumber() {
	return phoneNumber;
}

void ApplicationCall::runCallTimer() {
	callDuration = 0;
	callEndTime = -1;
	ReloadTimer(timerCall);
}

void ApplicationCall::destroyUserInterface() {
}

} /* namespace gui */
