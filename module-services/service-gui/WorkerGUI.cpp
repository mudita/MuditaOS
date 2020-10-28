// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

extern "C"
{
}

#include <Service/Bus.hpp> // for Bus
#include <memory>          // for unique_ptr, make_shared
#include <utility>         // for move
#include <vector>          // for vector

#include "Service/Service.hpp" // for Service
#include "Service/Message.hpp" // for DataMessage
#include "Service/Worker.hpp"  // for Worker, WorkerCommand
#include "MessageType.hpp"     // for MessageType, MessageType::GUIRenderingFinished
#include "WorkerGUI.hpp"
// module-utils
#include "log/log.hpp"                // for LOG_ERROR
#include "DrawCommand.hpp"            // for DrawCommand
#include "Renderer.hpp"               // for Renderer
#include "projdefs.h"                 // for pdMS_TO_TICKS, pdTRUE
#include "queue.h"                    // for xQueueReceive, QueueDefinition, QueueHandle_t
#include "semphr.h"                   // for xSemaphoreGive, xSemaphoreTake
#include "service-gui/ServiceGUI.hpp" // for ServiceGUI

namespace sgui
{

    WorkerGUI::WorkerGUI(ServiceGUI *service) : Worker(service)
    {}

    bool WorkerGUI::handleMessage(uint32_t queueID)
    {
        QueueHandle_t queue = queues[queueID];

        sgui::ServiceGUI *serviceGUI = reinterpret_cast<sgui::ServiceGUI *>(service);

        // queue for receiving rendering commands
        if (queueID == 0) {
            //		LOG_INFO("Received rendering commands");

            sys::WorkerCommand received;
            xQueueReceive(queue, &received, 0);

            // take all unique pointers
            std::vector<std::unique_ptr<gui::DrawCommand>> uniqueCommands;

            if (xSemaphoreTake(serviceGUI->semCommands, pdMS_TO_TICKS(1000)) == pdTRUE) {
                for (auto it = serviceGUI->commands.begin(); it != serviceGUI->commands.end(); it++)
                    uniqueCommands.push_back(std::move(*it));
                serviceGUI->commands.clear();
                xSemaphoreGive(serviceGUI->semCommands);
            }
            else {
                LOG_ERROR("Failed to acquire semaphore");
            }

            // create temporary vector of pointers to draw commands to avoid polluting renderer with smart pointers.
            std::vector<gui::DrawCommand *> commands;
            for (auto it = uniqueCommands.begin(); it != uniqueCommands.end(); it++)
                commands.push_back((*it).get());

            //		uint32_t start_tick = xTaskGetTickCount();
            serviceGUI->renderer.render(serviceGUI->renderContext, commands);
            //		uint32_t end_tick = xTaskGetTickCount();
            //		LOG_INFO("[WorkerGUI] RenderingTime: %d", end_tick - start_tick);

            //			delete received;

            // notify gui service that rendering is complete
            auto message = std::make_shared<sys::DataMessage>(MessageType::GUIRenderingFinished);
            sys::Bus::SendUnicast(message, this->service->GetName(), this->service);
        }
        return true;
    }

} /* namespace sgui */
