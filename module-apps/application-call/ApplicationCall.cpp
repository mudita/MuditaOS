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

#include "service-appmgr/ApplicationManager.hpp"

#include "ApplicationCall.hpp"
namespace app {

ApplicationCall::ApplicationCall(std::string name) :
	Application( name, 2048 ) {

	timer_id = CreateTimer(3000,true);

	busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);
}

ApplicationCall::~ApplicationCall() {
}

// Invoked upon receiving data message
sys::Message_t ApplicationCall::DataReceivedHandler(sys::DataMessage* msgl) {

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

		if (msg->type == CellularNotificationMessage::Type::CallAborted) {
		   LOG_INFO("CallAborted");
		}
		else if( msg->type == CellularNotificationMessage::Type::CallBusy) {
		   LOG_INFO("CallBusy");
		}
		else if( msg->type == CellularNotificationMessage::Type::CallActive ) {
		   LOG_INFO("CallActive");
		}
		else if( msg->type == CellularNotificationMessage::Type::IncomingCall ) {
		   LOG_INFO("IncomingCall 1 %s", msg->data.c_str());
		   std::unique_ptr<gui::SwitchData> data = std::make_unique<app::IncommingCallData>(msg->data);
		   LOG_INFO("IncomingCall 2");
		   //send to itself message to switch (run) call application
		   if( state == State::ACTIVE_FORGROUND ) {
			   LOG_INFO("FORGROUND");
		   }
		   else {
			   LOG_INFO("BACKGROUND");
			   sapm::ApplicationManager::messageSwitchApplication( this, "ApplicationCall", "CallWindow", std::move(data) );
//			   ApplicationManager::messageSwitchApplication( this, "ApplicationCall", "CallWindow", std::move(data) );
		   }
		}
		else if( msg->type == CellularNotificationMessage::Type::NewIncomingSMS ) {
		   LOG_INFO("NewIncomingSMS");
		}
		else if( msg->type == CellularNotificationMessage::Type::SignalStrengthUpdate ) {
		   LOG_INFO("SignalStrengthUpdate");
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

sys::ReturnCodes ApplicationCall::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ApplicationCall::SleepHandler() {
	return sys::ReturnCodes::Success;
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

void ApplicationCall::destroyUserInterface() {
}

} /* namespace gui */
