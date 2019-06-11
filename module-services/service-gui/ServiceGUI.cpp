/*
 * ServiceGUI.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include <memory>

//module-os
#include "FreeRTOS.h"
#include "semphr.h"

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

extern "C"
#include "module-os/memory/usermem.h"

#include "SystemManager/SystemManager.hpp"
#include "WorkerGUI.hpp"

namespace sgui {

ServiceGUI::ServiceGUI(const std::string& name, uint32_t screenWidth, uint32_t screenHeight)
		: sys::Service(name, 4096, sys::ServicePriority::Idle),
		renderContext{ nullptr },
		transferContext { nullptr },
		renderFrameCounter{ 1 },
		transferedFrameCounter{ 0 },
		screenWidth{ screenWidth },
		screenHeight{ screenHeight },
		semCommands{ NULL },
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
			(mode==gui::RefreshModes::GUI_REFRESH_DEEP?true:false),
			transferContext->getData()
	);
	einkReady = false;
	auto ret = sys::Bus::SendUnicast(msg, "ServiceEink", this,2000);
	if( ret.first == sys::ReturnCodes::Success ) {
		transferedFrameCounter = renderFrameCounter;
	}
	//set default refreshing mode.
	 mode = gui::RefreshModes::GUI_REFRESH_FAST;

}

void ServiceGUI::sendToRender() {
	rendering = true;
	worker->send(static_cast<uint32_t>(WorkerGUICommands::Render), NULL);
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

				//update mode
				if( dmsg->mode == gui::RefreshModes::GUI_REFRESH_DEEP ) {
					mode = dmsg->mode;
				}

				LOG_INFO("[ServiceGUI] Received %d draw commands", dmsg->commands.size());

				//lock access to commands vector, clear it and then copy commands from message to vector
				if( xSemaphoreTake( semCommands, pdMS_TO_TICKS(1000) ) == pdTRUE ) {
					commands.clear();
					for (auto it = dmsg->commands.begin(); it != dmsg->commands.end(); it++)
						commands.push_back(std::move(*it));
					xSemaphoreGive( semCommands );
				}

				//if worker is not rendering send him new set of commands
				if( !rendering ) {
					sendToRender();
				}

				uint32_t mem = usermemGetFreeHeapSize();
				LOG_WARN( "Heap Memory: %d", mem );
			}

		} break;
		case static_cast<uint32_t>(MessageType::GUIRenderingFinished): {
			LOG_INFO("Rendering finished");
			//increment counter holding number of drawn frames
			rendering = false;
			renderFrameCounter++;
			//copy render buffer to transfer buffer using semaphore to protect data
			//gui service is locking semaphore, makes a copy and then sends message to eink

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
			//mode = gui::RefreshModes::GUI_REFRESH_FAST;
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
}

// Invoked during initialization
sys::ReturnCodes ServiceGUI::InitHandler() {

	//create semaphore to protect vector with commands waiting for rendering
	semCommands = xSemaphoreCreateBinary();
	if( semCommands == NULL ) {
		LOG_FATAL("Failed to create commands semaphore.");
		return sys::ReturnCodes::Failure;
	}
	xSemaphoreGive( semCommands );

	//initialize gui worker
	worker = new WorkerGUI( this );
	std::list<sys::WorkerQueueInfo> list;
	worker->init( list );
	worker->run();

	if( einkReady == false ) {
		requestSent = true;
		auto msg = std::make_shared<seink::EinkMessage>(MessageType::EinkStateRequest );
		sys::Bus::SendUnicast(msg, "ServiceEink", this);
	}
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceGUI::DeinitHandler() {

	if( semCommands != NULL )
		vSemaphoreDelete( semCommands );
	semCommands = NULL;

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceGUI::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceGUI::SleepHandler() {
	return sys::ReturnCodes::Success;
}

} /* namespace sgui */


