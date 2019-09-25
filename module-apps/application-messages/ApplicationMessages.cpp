/*
 * @file ApplicationMessages.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 25 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "Application.hpp"

#include "MessageType.hpp"
#include "windows/MessagesMainWindow.hpp"

#include "ApplicationMessages.hpp"

namespace app {

ApplicationMessages::ApplicationMessages(std::string name, std::string parent, bool startBackgound) :
    Application( name, parent, startBackgound, 4096 ) {
}

ApplicationMessages::~ApplicationMessages() {}

// Invoked upon receiving data message
sys::Message_t ApplicationMessages::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage* resp) {

    auto retMsg = Application::DataReceivedHandler(msgl);
    // if message was handled by application's template there is no need to process further.
    if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
        return retMsg;
    }

    // this variable defines whether message was processed.
    bool handled = false;

    //handle database response
	if( resp != nullptr ) {
		handled = true;
		uint32_t msgType = resp->responseTo;
		switch( msgType ) {
			case static_cast<uint32_t>(MessageType::DBContactGetLimitOffset): {
				if( currentWindow->onDatabaseMessage( resp ) )
					refreshWindow( gui::RefreshModes::GUI_REFRESH_FAST );
			}break;
		}
	}

    if (handled)
        return std::make_shared<sys::ResponseMessage>();
    else
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ApplicationMessages::InitHandler() {

    auto ret = Application::InitHandler();
    if (ret != sys::ReturnCodes::Success)
        return ret;

    createUserInterface();

    setActiveWindow("MainWindow");

    return ret;
}

sys::ReturnCodes ApplicationMessages::DeinitHandler() { return sys::ReturnCodes::Success; }

void ApplicationMessages::createUserInterface() {

    gui::AppWindow *window = nullptr;

    window = new gui::MessagesMainWindow(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
}

void ApplicationMessages::destroyUserInterface() {}

} /* namespace app */
