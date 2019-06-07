/*
 * @file GUIWorker.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 31 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <string.h>

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
}


#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Worker.hpp"
#include "MessageType.hpp"
#include "WorkerGUI.hpp"
//module-utils
#include "log/log.hpp"

namespace sgui {

WorkerGUI::WorkerGUI( ServiceGUI* service ) : Worker( service ) {

}

WorkerGUI::~WorkerGUI() {
}

bool WorkerGUI::handleMessage( uint32_t queueID ) {
	QueueHandle_t queue = queues[queueID];

	//queue for receiving rendering commands
	if( queueID == 0 )
	{
		LOG_INFO("Received rendering commands");

		WorkerGUIBufferData* received;
		xQueueReceive(queue, &received, 0);
		if( received != nullptr);

		//create temporary vector of pointers to draw commands to avoid polluting renderer with smart pointers.
		std::vector<gui::DrawCommand*> commands;
		for (auto i = service->latestCommands.begin(); i!=service->latestCommands.end(); ++i)
			commands.push_back( (*i).get() );

		uint32_t start_tick = xTaskGetTickCount();
		renderer.render( received->context, commands );
		uint32_t end_tick = xTaskGetTickCount();
		LOG_INFO("[WorkerGUI] RenderingTime: %d", end_tick - start_tick);

		delete received;

		//notify gui service that rendering is complete
		auto message = std::make_shared<sys::DataMessage>(static_cast<uint32_t>(MessageType::GUIRenderingFinished));
		sys::Bus::SendUnicast(message, this->service->GetName(), this->service);
	}
	return true;
}

} /* namespace sgui */
