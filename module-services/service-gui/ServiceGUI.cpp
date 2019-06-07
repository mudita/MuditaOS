/*
 * ServiceGUI.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include <memory>

//module-gui
#include "gui/core/Font.hpp"
#include "gui/core/Context.hpp"

//gui service
#include "messages/GUIMessage.hpp"
#include "messages/DrawMessage.hpp"

//service-eink
#include "service-eink/messages/ImageMessage.hpp"

#include "ServiceGUI.hpp"

#include "..//gui/core/ImageManager.hpp"
#include "log/log.hpp"

#include "SystemManager/SystemManager.hpp"
#include "WorkerGUI.hpp"

namespace sgui {

ServiceGUI::ServiceGUI(const std::string& name, uint32_t screenWidth, uint32_t screenHeight)
		: sys::Service(name, 4096, sys::ServicePriority::High),
		renderContext{ nullptr },
		transferContext { nullptr },
		renderFrameCounter{ 1 },
		transferedFrameCounter{ 0 },
		screenWidth{ screenWidth },
		screenHeight{ screenHeight },
		worker{nullptr} {

	LOG_INFO("[ServiceGUI] Initializing");

	//allocate buffers for rendering and transferring data to eink
	renderContext = new gui::Context( screenWidth, screenHeight );
	transferContext = new gui::Context( screenWidth, screenHeight );

	gui::FontManager& fontManager = gui::FontManager::getInstance();
	fontManager.init( "sys/assets" );

	gui::ImageManager& imageManager = gui::ImageManager::getInstance();
	imageManager.init( "sys/assets" );
}

ServiceGUI::~ServiceGUI(){
	LOG_INFO("[ServiceGUI] Cleaning resources");
	if( renderContext )
		delete renderContext;
	if( transferContext )
		delete transferContext;
}

void ServiceGUI::sendBuffer() {
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
	einkReady = false;
}

void ServiceGUI::sendToRender() {
	if( rendering )
		return;

	WorkerGUIBufferData* wgc = new WorkerGUIBufferData();
	wgc->commandsCount = latestCommands.size();
//	for (uint32_t i = 0; i != latestCommands.size(); ++i){
//		wgc->commands.push_back( std::move(latestCommands[i]));
//	}
//
//	latestCommands.clear();
	wgc->context = renderContext;
	worker->send(static_cast<uint32_t>(WorkerGUICommands::Render), nullptr);


	rendering = true;
}


// Invoked upon receiving data message
sys::Message_t ServiceGUI::DataReceivedHandler(sys::DataMessage* msgl) {

	sgui::GUIMessage* msg = static_cast<sgui::GUIMessage*>(msgl);

	switch( msg->messageType ) {
		case static_cast<uint32_t>(MessageType::MessageTypeUninitialized): {
			LOG_ERROR("[ServiceGUI] Received uninitialized message type");
		} break;
		case static_cast<uint32_t>( MessageType::GUICommands ): {
			auto dmsg = static_cast<sgui::DrawMessage*>( msgl );
			if( !dmsg->commands.empty() ) {
				LOG_INFO("[ServiceGUI] Received %d draw commands", dmsg->commands.size());

				//create temporary vector of pointers to draw commands to avoid polluting renderer with smart pointers.
//				std::vector<gui::DrawCommand*> commands;
//				for (auto i = dmsg->commands.begin(); i != dmsg->commands.end(); ++i)
//					commands.push_back( (*i).get() );

				if( !latestCommands.empty())
					latestCommands.clear();

				//store commands in the latestCommands container
				for (auto i = dmsg->commands.begin(); i != dmsg->commands.end(); ++i)
					latestCommands.push_back( std::move(*i));

				if( (renderFrameCounter != transferedFrameCounter) &&
					(!rendering) &&
					einkReady) {
					LOG_INFO("[ServiceGUI]Sending buffer");
					sendBuffer();
				}

				//if worker is not rendering send him new set of commands
				if( !rendering ) {
					sendToRender();
				}

//				uint32_t start_tick = xTaskGetTickCount();
//				renderer.render( renderContext, commands );
//				uint32_t end_tick = xTaskGetTickCount();
//				LOG_INFO("[ServiceGUI] RenderingTime: %d", end_tick - start_tick);
			}

		} break;
		case static_cast<uint32_t>(MessageType::GUIRenderingFinished): {
			LOG_INFO("Rendering finished");
			//increment counter holding number of drawn frames
			renderFrameCounter++;

			if( einkReady ) {
				sendBuffer();
			}
			else if( !requestSent ){
				requestSent = true;
				//request eink state
				auto msg = std::make_shared<seink::EinkMessage>(MessageType::EinkStateRequest);
				sys::Bus::SendUnicast(msg, "ServiceEink", this);
			}
		}break;
		case static_cast<uint32_t>( MessageType::GUIFocusInfo ): {

			LOG_INFO("[ServiceGUI] Received focus info");
		} break;
		case static_cast<uint32_t>( MessageType::GUIDisplayReady ): {

			LOG_INFO("[ServiceGUI]Display ready");
			einkReady = true;
			requestSent = false;
			//check if something new was rendered. If so render counter has greater value than
			//transfer counter.
			if( (renderFrameCounter != transferedFrameCounter) && (!rendering) ) {
				LOG_INFO("[ServiceGUI]Sending buffer");
				sendBuffer();
			}
			else {
				LOG_INFO(" NO new buffer to send");
			}

		} break;
	};

	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceGUI::TickHandler(uint32_t id) {
	if( einkReady == false ) {
		ReloadTimer( timer_id );
		auto msg = std::make_shared<seink::EinkMessage>(MessageType::EinkStateRequest );
		sys::Bus::SendUnicast(msg, "ServiceEink", this);
	}
}

// Invoked during initialization
sys::ReturnCodes ServiceGUI::InitHandler() {

	//initialize keyboard worker
	worker = new WorkerGUI( this );
	std::list<sys::WorkerQueueInfo> list;
	worker->init( list );
	worker->run();

	if( einkReady == false ) {
		requestSent = true;
		ReloadTimer( timer_id );
		auto msg = std::make_shared<seink::EinkMessage>(MessageType::EinkStateRequest );
		sys::Bus::SendUnicast(msg, "ServiceEink", this);
	}
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

} /* namespace sgui */


