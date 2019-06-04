/*
 * @file Application.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
//module-utils
#include "log/log.hpp"
//module-services
#include "service-gui/messages/DrawMessage.hpp"
//module-gui
#include "gui/core/DrawCommand.hpp"
//module-sys
#include "SystemManager/SystemManager.hpp"


#include "Application.hpp"

namespace app {

Application::Application(std::string name,uint32_t stackDepth,sys::ServicePriority priority) :
	Service( name, stackDepth, priority ){
	// TODO Auto-generated constructor stub

}

Application::~Application() {
	// TODO Auto-generated destructor stub
}

void Application::render( gui::RefreshModes mode ) {
	if( currentWindow == nullptr ) {
		LOG_ERROR("Current window is not defined");
		return;
	}

	std::list<gui::DrawCommand*> commandsList = currentWindow->buildDrawList();
	auto msg = std::make_shared<sgui::DrawMessage>(commandsList, mode);
	sys::Bus::SendUnicast(msg, "ServiceGUI", this);
}
void Application::blockEvents(bool isBlocked ) {
	acceptInput = isBlocked;
}
int Application::switchWindow( const std::string& windowName, uint32_t cmd, uint32_t dataSize, uint8_t* data ) {
	return 0;
}
int Application::switchBackWindow( const std::string& windowName, uint32_t cmd, uint32_t dataSize, uint8_t* data ) {
	return 0;
}
int Application::refreshWindow(gui::RefreshModes mode) {
	return 0;
}
void Application::setActiveWindow( const std::string& windowName ) {
	auto it = windows.find(windowName);

	//if there is a window with specified name set it as active window
	//and unlock accepting keyboard events
	if( it!=windows.end() ) {
		previousWindow = currentWindow;
		currentWindow = it->second;
		acceptInput = true;
	}
}

} /* namespace app */
