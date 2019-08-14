/*
 * @file ApplicationNotes.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 30 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "MessageType.hpp"
#include "windows/NotesMainWindow.hpp"
#include "windows/NotesEditWindow.hpp"
#include "service-db/messages/DBMessage.hpp"

#include "ApplicationNotes.hpp"

namespace app {

ApplicationNotes::ApplicationNotes(std::string name, bool startBackgound) :
	Application( name, startBackgound, 2048 ) {
}

ApplicationNotes::~ApplicationNotes() {
}

// Invoked upon receiving data message
sys::Message_t ApplicationNotes::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

	auto retMsg = Application::DataReceivedHandler(msgl);
	//if message was handled by application's template there is no need to process further.
	if( (reinterpret_cast<sys::ResponseMessage*>( retMsg.get() )->retCode ==
		sys::ReturnCodes::Success ) ){
		return retMsg;
	}

	bool handled = false;

	//handle database response
	if( resp != nullptr ) {
		uint32_t msgType = resp->responseTo;
		switch( msgType ) {
			case static_cast<uint32_t>(MessageType::DBNotesGetLimitOffset): {
				if( currentWindow->onDatabaseMessage( resp ) )
					refreshWindow( gui::RefreshModes::GUI_REFRESH_FAST );
			}break;
		}
	}

	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ApplicationNotes::InitHandler() {

	auto ret = Application::InitHandler();
	if( ret != sys::ReturnCodes::Success )
		return ret;

	createUserInterface();

	setActiveWindow("MainWindow");

	return ret;
}

sys::ReturnCodes ApplicationNotes::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationNotes::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ApplicationNotes::SleepHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationNotes::createUserInterface() {

	gui::AppWindow* window = nullptr;

	window = new gui::NotesMainWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::NotesEditWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));
}

void ApplicationNotes::destroyUserInterface() {
}

} /* namespace app */
