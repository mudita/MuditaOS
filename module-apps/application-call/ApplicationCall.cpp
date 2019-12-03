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
	Application( name, parent, startBackgound, 4096+2048 ) {

	timerCallId = CreateTimer(1000,true);
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
			callDuration = 0;

			LOG_INFO("---------------------------------CallActive");
			callWindow->setState( gui::CallWindow::State::CALL_IN_PROGRESS );
			refreshWindow( gui::RefreshModes::GUI_REFRESH_DEEP );
		}
		else if( msg->type == CellularNotificationMessage::Type::IncomingCall ) {
			//reset call duration
//		    callDuration = 0;
			LOG_INFO("---------------------------------IncomingCall");
			if( callWindow->getState() == gui::CallWindow::State::INCOMING_CALL ) {
				LOG_INFO("ignoring call incoming");
			}
			else {

				AudioServiceAPI::RoutingStart(this);
				runCallTimer();
				std::unique_ptr<gui::SwitchData> data = std::make_unique<app::IncommingCallData>(msg->data);
				//send to itself message to switch (run) call application
				callWindow->setState( gui::CallWindow::State::INCOMING_CALL );
				if( state == State::ACTIVE_FORGROUND ) {
					LOG_INFO("++++++++++++WINDOW SWITCH");
					switchWindow( "CallWindow", std::move(data) );
				}
				else {
					LOG_INFO("++++++++++++APP SWITCH");

					sapm::ApplicationManager::messageSwitchApplication( this, "ApplicationCall", "CallWindow", std::move(data) );
				}
			}
		}
		else if( msg->type == CellularNotificationMessage::Type::Ringing ) {
			//reset call duration
		    //callDuration = 0;
			runCallTimer();
			LOG_INFO("---------------------------------Ringing");
			AudioServiceAPI::RoutingStart(this);

			std::unique_ptr<gui::SwitchData> data = std::make_unique<app::ExecuteCallData>(msg->data);
			callWindow->setState( gui::CallWindow::State::OUTGOING_CALL );
			if( state == State::ACTIVE_FORGROUND ) {
				switchWindow( "CallWindow", std::move(data) );
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
	if (id == timerCallId)
	{
		++callDuration;

		auto it = windows.find("CallWindow");
		if( currentWindow == it->second ) {
			gui::CallWindow* callWindow = reinterpret_cast<gui::CallWindow*>(currentWindow);

			if( callWindow->getState() == gui::CallWindow::State::CALL_IN_PROGRESS ) {
				callWindow->updateDuration( callDuration );
				refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
			}
		}

		LOG_INFO("callDuration %d, callEndTime id %d", callDuration, callEndTime);
		if( callDuration >= callEndTime ) {
			stopTimer(timerCallId);
			sapm::ApplicationManager::messageSwitchPreviousApplication( this );
		}
	}
	else
	{
        Application::TickHandler(id);
    }
}

void ApplicationCall::stopCallTimer() {
	LOG_INFO("switching to prev calldur: %d endTime: %d", callDuration, callEndTime );
	stopTimer(timerCallId);
	sapm::ApplicationManager::messageSwitchPreviousApplication( this );
}

bool ApplicationCall::messageSwitchToCall( sys::Service* sender, const UTF8& e164number, bool call ) {
	std::string application = "ApplicationCall";
	std::string window = "CallWindow";

	std::unique_ptr<ExecuteCallData> data = std::make_unique<ExecuteCallData>(e164number.c_str());

	//it is possible to prepare call window and wait for user's acceptance
	if( call )
		data->setDescription("call"); //execute call
	else
		data->setDescription("prepare"); //TODO prepare call and wait for user's action

	return sapm::ApplicationManager::messageSwitchApplication( sender, application, window, std::move(data));
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
	ReloadTimer(timerCallId);
}

void ApplicationCall::destroyUserInterface() {
}

} /* namespace gui */
