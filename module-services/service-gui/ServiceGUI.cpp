/*
 * ServiceGUI.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "gui/core/Font.hpp"
#include "gui/core/PixMapManager.hpp"
//gui service
#include "GUIMessage.hpp"
#include "DrawMessage.hpp"

#include "ServiceGUI.hpp"
#include "log/log.hpp"

ServiceGUI::ServiceGUI(const std::string& name)
		: sys::Service(name),
		renderBuffer { nullptr },
		einkBuffer { nullptr } {

	LOG_INFO("[ServiceGUI] Initializing");

	gui::FontManager& fontManager = gui::FontManager::getInstance();
	fontManager.init( "sys/assets" );

	gui::PixMapManager& pixMapManager = gui::PixMapManager::getInstance();
	pixMapManager.init( "sys/assets" );
}

ServiceGUI::~ServiceGUI(){
	LOG_INFO("[ServiceGUI] Cleaning resources");
}

// Invoked upon receiving data message
sys::Message_t ServiceGUI::DataReceivedHandler(sys::DataMessage* msgl) {

	sgui::GUIMessage* msg = static_cast<sgui::GUIMessage*>(msgl);

	switch( msg->messageType ) {
	case sgui::GUIMessageType::Uninitialized: {
		LOG_ERROR("[ServiceGUI] Received uninitialized message type");
	} break;
	case sgui::GUIMessageType::Commands: {
		auto dmsg = static_cast<sgui::DrawMessage*>( msgl );
		LOG_INFO("[ServiceGUI] Received %d draw commands", dmsg->commands.size());
	} break;
	case sgui::GUIMessageType::FocusInfo: {

		LOG_INFO("[ServiceGUI] Received focus info");
	} break;
	};

	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceGUI::TickHandler(uint32_t id) {
}

// Invoked during initialization
sys::ReturnCodes ServiceGUI::InitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceGUI::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceGUI::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceGUI::SleepHandler() {
	return sys::ReturnCodes::Success;
}




