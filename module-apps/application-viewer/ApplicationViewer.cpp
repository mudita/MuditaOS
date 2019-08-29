/*
 * @file ApplicationViewer.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include <vector>
#include <string>
#include <sstream>

//module-gui
#include "ApplicationViewer.hpp"

#include "service-appmgr/ApplicationManager.hpp"

#include "gui/widgets/Window.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
//module-utils
#include "log/log.hpp"
//module-services
#include "service-evtmgr/EventManager.hpp"
#include "service-evtmgr/messages/EVMessages.hpp"
//MessageType
#include "MessageType.hpp"
//this module
#include "windows/ViewWindow.hpp"

namespace app {

ApplicationViewer::ApplicationViewer(std::string name, std::string parent,uint32_t stackDepth,sys::ServicePriority priority) :
	Application( name, parent, false, stackDepth, priority ) {
}

ApplicationViewer::~ApplicationViewer() {
	// TODO Auto-generated destructor stub
}

// Invoked upon receiving data message
sys::Message_t ApplicationViewer::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

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
sys::ReturnCodes ApplicationViewer::InitHandler() {

	auto ret = Application::InitHandler();
	if( ret != sys::ReturnCodes::Success )
		return ret;

	createUserInterface();

	setActiveWindow("Main");

	return ret;
}

sys::ReturnCodes ApplicationViewer::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationViewer::createUserInterface() {

	gui::AppWindow* window = nullptr;

	window = new gui::ViewWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));
}

void ApplicationViewer::destroyUserInterface() {
}

} /* namespace app */
