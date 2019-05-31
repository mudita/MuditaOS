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

ServiceGUI::ServiceGUI(const std::string& name, uint32_t screenWidth, uint32_t screenHeight)
		: sys::Service(name),
		renderBuffer { nullptr },
		transferBuffer { nullptr },
		renderFrameCounter{ 1 },
		transferedFrameCounter{ 0 },
		screenWidth{ screenWidth },
		screenHeight{ screenHeight } {

	LOG_INFO("[ServiceGUI] Initializing");

	//allocate buffers for rendering and transferring data to eink
	renderBuffer = new FrameBuffer( screenWidth, screenHeight );
	transferBuffer = new FrameBuffer( screenWidth, screenHeight );

	gui::FontManager& fontManager = gui::FontManager::getInstance();
	fontManager.init( "sys/assets" );

	gui::PixMapManager& pixMapManager = gui::PixMapManager::getInstance();
	pixMapManager.init( "sys/assets" );
}

ServiceGUI::~ServiceGUI(){
	LOG_INFO("[ServiceGUI] Cleaning resources");
	if( renderBuffer )
		delete renderBuffer;
	if( transferBuffer )
		delete transferBuffer;
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
	///	LOG_INFO("[ServiceGUI] Received %d draw commands", dmsg->commands.size());
	} break;
	case sgui::GUIMessageType::FocusInfo: {

	//	LOG_INFO("[ServiceGUI] Received focus info");
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




