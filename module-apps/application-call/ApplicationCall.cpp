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

#include "ApplicationCall.hpp"
namespace app {

ApplicationCall::ApplicationCall(std::string name) :
	Application( name, 8192 ) {
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
	bool handled = true;

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

	gui::Window* window = nullptr;

	window = new gui::CallMainWindow(this);
	windows.insert(std::pair<std::string,gui::Window*>(window->getName(), window));

	window = new gui::EnterNumberWindow(this);
	windows.insert(std::pair<std::string,gui::Window*>(window->getName(), window));

	window = new gui::EmergencyCallWindow(this);
	windows.insert(std::pair<std::string,gui::Window*>(window->getName(), window));
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
