/*
 * @file ApplicationCallLog.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 19.09.2019
 * @brief Application Call Log
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "MessageType.hpp"
#include "windows/CallLogMainWindow.hpp"
#include "windows/CallLogDetailsWindow.hpp"
#include "windows/CallLogOptionsWindow.hpp"
#include "windows/CallLogCallDeleteWindow.hpp"
#include "data/CallLogInternals.hpp"
#include "service-db/messages/DBMessage.hpp"

#include "ApplicationCallLog.hpp"

using namespace calllog;

namespace app {

ApplicationCallLog::ApplicationCallLog(std::string name, std::string parent, bool startBackgound) :
	Application( name, parent, startBackgound, 4096 ) {
}

ApplicationCallLog::~ApplicationCallLog() {
	LOG_INFO("ApplicationCallLog::destroy");
}

// Invoked upon receiving data message
sys::Message_t ApplicationCallLog::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

	auto retMsg = Application::DataReceivedHandler(msgl);
	//if message was handled by application's template there is no need to process further.
	if( (reinterpret_cast<sys::ResponseMessage*>( retMsg.get() )->retCode == sys::ReturnCodes::Success ) ){
		return retMsg;
	}

	auto handled = false;

	//handle database response
	if( resp != nullptr ) {
		handled = true;
		uint32_t msgType = resp->responseTo;
		switch( msgType ) {
			case static_cast<uint32_t>(MessageType::DBCalllogGetLimitOffset): {
                if (getCurrentWindow()->onDatabaseMessage(resp))
                {
                    refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                }
                break;
            }
		}
	}

	if( handled ) {
		return std::make_shared<sys::ResponseMessage>();
	}
	else {
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
	}
}

// Invoked during initialization
sys::ReturnCodes ApplicationCallLog::InitHandler() {
	auto ret = Application::InitHandler();
	if(ret != sys::ReturnCodes::Success) return ret;

	createUserInterface();

	setActiveWindow(calllog::settings::MainWindowStr);

	return ret;
}

sys::ReturnCodes ApplicationCallLog::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationCallLog::createUserInterface() {

	gui::AppWindow* window = nullptr;

	window = new gui::CallLogMainWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::CallLogDetailsWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::CallLogOptionsWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::CallLogCallDeleteWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));
}

void ApplicationCallLog::destroyUserInterface() {
}

} /* namespace app */
