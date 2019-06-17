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
#include "service-appmgr/messages/APMMessage.hpp"
#include "service-appmgr/ApplicationManager.hpp"
//module-gui
#include "gui/core/DrawCommand.hpp"
//module-sys
#include "SystemManager/SystemManager.hpp"

#include "service-db/api/DBServiceAPI.hpp"


#include "Application.hpp"
#include "MessageType.hpp"
#include "messages/AppMessage.hpp"

namespace app {

Application::Application(std::string name,uint32_t stackDepth,sys::ServicePriority priority) :
	Service( name, stackDepth, priority ){

	// TODO Auto-generated constructor stub

}

Application::~Application() {
	// TODO Auto-generated destructor stub
}

void Application::render( gui::RefreshModes mode ) {
	if( currentWindow == nullptr ){
		LOG_ERROR("Current window is not defined");
		return;
	}
	//send drawing commands only when if application is in active and visible.
	if( state == State::ACTIVE_FORGROUND ) {
		std::list<gui::DrawCommand*> commandsList = currentWindow->buildDrawList();
		auto msg = std::make_shared<sgui::DrawMessage>(commandsList, mode);
		sys::Bus::SendUnicast(msg, "ServiceGUI", this);
	}
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

sys::Message_t Application::DataReceivedHandler(sys::DataMessage* msgl) {
	bool handled = false;
	if(msgl->messageType == static_cast<uint32_t>(MessageType::AppSwitch) ) {
		//
		if( (state == State::INITIALIZING ) || ( state == State::ACTIVE_BACKGROUND ) ){
			state = State::ACTIVATING;

			if( sapm::ApplicationManager::messageConfirmSwitch(this) ) {
				state = State::ACTIVE_FORGROUND;
				render( gui::RefreshModes::GUI_REFRESH_DEEP );
			}
			else {
				//TODO send to itself message to close
				LOG_ERROR("Failed to communicate ");
			}

		}
		else {
			LOG_ERROR("Wrong internal application to switch to active state");
		}
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::AppSwitchWithData) ) {

	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::AppFocus ) ) {

	}
	else if( msgl->messageType == static_cast<uint32_t>(MessageType::AppClose)) {
		state = State::DEACTIVATING;
		sapm::ApplicationManager::messageConfirmClose(this);
		//here should go all the cleaning
	}

	if( handled)
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

sys::ReturnCodes Application::InitHandler() {
	bool initState= true;
	state = State::INITIALIZING;
	settings = DBServiceAPI::SettingsGet(this);
	initState = (settings.dbID == 1);

	//send response to application manager true if successful, false otherwise.
	auto msg = std::make_shared<sapm::APMRegister>(this->GetName(), initState );
	sys::Bus::SendUnicast(msg, "ApplicationManager", this);


	return (initState?sys::ReturnCodes::Success:sys::ReturnCodes::Failure);
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

bool Application::messageSwitchApplication( sys::Service* sender, std::string application, std::string window ) {
	auto msg = std::make_shared<AppMessage>( MessageType::AppSwitch, sender->GetName() );
	sys::Bus::SendUnicast(msg, application, sender );
	return true;
}

bool Application::messageSwitchApplicationWithData( sys::Service* sender, std::string application, std::string window, SwitchData* data ) {
	auto msg = std::make_shared<AppMessage>( MessageType::AppSwitchWithData, sender->GetName() );
	sys::Bus::SendUnicast(msg, application, sender );
	return true;
}

bool Application::messageRefreshApplication( sys::Service* sender, std::string application, std::string window, SwitchData* data ) {
	auto msg = std::make_shared<AppMessage>( MessageType::AppRefresh, sender->GetName() );
	sys::Bus::SendUnicast(msg, application, sender );
	return true;
}

bool Application::messageCloseApplication( sys::Service* sender, std::string application ) {

	auto msg = std::make_shared<AppMessage>( MessageType::AppClose, sender->GetName() );
	sys::Bus::SendUnicast(msg, application, sender );
	return true;
}

bool Application::messageFocusApplication( sys::Service* sender, std::string application, bool focus ) {
	auto msg = std::make_shared<AppFocusMessage>( application, focus );
	auto ret = sys::Bus::SendUnicast(msg, application, sender, 1000 );
	return (ret.first == sys::ReturnCodes::Success )?true:false;
}

//	{
//
//
//	}


} /* namespace app */
