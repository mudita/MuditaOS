// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerGUI.hpp"

#include <DrawCommand.hpp>
#include <log/log.hpp>
#include <MessageType.hpp>
#include <projdefs.h>
#include <Renderer.hpp>
#include <Service/Bus.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <service-gui/ServiceGUI.hpp>

#include <memory>
#include <utility>
#include <vector>
#include "messages/RenderingFinished.hpp"

#include <semphr.h>
#include <queue.h>

namespace sgui
{

    WorkerGUI::WorkerGUI(ServiceGUI *service) : Worker(service), service(service)
    {}

    bool WorkerGUI::handleMessage(uint32_t queueID)
    {
        auto &queue = queues[queueID];

        auto serviceGUI = static_cast<sgui::ServiceGUI *>(service);

        if (queueID == 0) {
            sys::WorkerCommand received;
            queue->Dequeue(&received, 0);

            std::list<std::unique_ptr<gui::DrawCommand>> uniqueCommands;
            if (xSemaphoreTake(serviceGUI->semCommands, pdMS_TO_TICKS(1000)) == pdTRUE) {
                uniqueCommands = std::move(serviceGUI->commands);
                xSemaphoreGive(serviceGUI->semCommands);
            }
            else {
                LOG_ERROR("Failed to acquire semaphore");
            }

            serviceGUI->renderer.render(serviceGUI->renderContext, uniqueCommands);

            sys::Bus::SendUnicast(std::make_shared<service::gui::RenderingFinished>(), service->GetName(), service);
        }
        return true;
    }

} /* namespace sgui */
