/*
 * ServiceGUI.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include <memory>

//module-gui
#include "gui/core/Font.hpp"
#include "gui/core/PixMapManager.hpp"
#include "gui/core/Context.hpp"

//gui service
#include "messages/GUIMessage.hpp"
#include "messages/DrawMessage.hpp"

//service-eink
#include "service-eink/messages/ImageMessage.hpp"

#include "ServiceGUI.hpp"
#include "log/log.hpp"

#include "SystemManager/SystemManager.hpp"

ServiceGUI::ServiceGUI(const std::string& name, uint32_t screenWidth, uint32_t screenHeight)
		: sys::Service(name),
		renderContext{ nullptr },
		transferContext { nullptr },
		renderFrameCounter{ 1 },
		transferedFrameCounter{ 0 },
		screenWidth{ screenWidth },
		screenHeight{ screenHeight } {

	LOG_INFO("[ServiceGUI] Initializing");

	//allocate buffers for rendering and transferring data to eink
	renderContext = new gui::Context( screenWidth, screenHeight );
	transferContext = new gui::Context( screenWidth, screenHeight );

	gui::FontManager& fontManager = gui::FontManager::getInstance();
	fontManager.init( "sys/assets" );

	gui::PixMapManager& pixMapManager = gui::PixMapManager::getInstance();
	pixMapManager.init( "sys/assets" );
}

ServiceGUI::~ServiceGUI(){
	LOG_INFO("[ServiceGUI] Cleaning resources");
	if( renderContext )
		delete renderContext;
	if( transferContext )
		delete transferContext;
}

// Invoked upon receiving data message
sys::Message_t ServiceGUI::DataReceivedHandler(sys::DataMessage* msgl) {

	sgui::GUIMessage* msg = static_cast<sgui::GUIMessage*>(msgl);

	switch( msg->messageType ) {
	case sgui::MessageType::Uninitialized: {
		LOG_ERROR("[ServiceGUI] Received uninitialized message type");
	} break;
	case sgui::MessageType::Commands: {
		auto dmsg = static_cast<sgui::DrawMessage*>( msgl );
		if( !dmsg->commands.empty() ) {
			LOG_INFO("[ServiceGUI] Received %d draw commands", dmsg->commands.size());

			//create temporary vector of pointers to draw commands to avoid polluting renderer with smart pointers.
			std::vector<gui::DrawCommand*> commands;
			for (auto i = dmsg->commands.begin(); i != dmsg->commands.end(); ++i)
				commands.push_back( (*i).get() );
			renderer.render( renderContext, commands );

			//increment counter holding number of drawn frames
			renderFrameCounter++;
		}

	} break;
	case sgui::MessageType::FocusInfo: {

		LOG_INFO("[ServiceGUI] Received focus info");
	} break;
	case sgui::MessageType::DisplayReady: {

		LOG_INFO("[ServiceGUI] Received display device ready signal");

		einkReady = true;
		DeleteTimer( timer_id );

		//check if something new was rendered. If so render counter has greater value than
		//transfer counter.
		if( renderFrameCounter != transferedFrameCounter ) {

			//copy data from render context to transfer context
			transferContext->insert( 0, 0, renderContext );

			auto msg = std::make_shared<seink::ImageMessage>( 0, 0,
					transferContext->getW(),
					transferContext->getH(),
					transferContext->getData()
			);
			auto ret = sys::Bus::SendUnicast(msg, "ServiceEink", this, 500);
			if( ret.first == sys::ReturnCodes::Success ) {
				transferedFrameCounter = renderFrameCounter;
			}
		}

	} break;
	};

	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceGUI::TickHandler(uint32_t id) {
	if( einkReady == false ) {
		ReloadTimer( timer_id );
		auto msg = std::make_shared<seink::EinkMessage>(seink::MessageType::StateRequest );
		sys::Bus::SendUnicast(msg, "ServiceEink", this);
	}
}

// Invoked during initialization
sys::ReturnCodes ServiceGUI::InitHandler() {
	timer_id = CreateTimer(100,true);
	ReloadTimer(timer_id);
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




