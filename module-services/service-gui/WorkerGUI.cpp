// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerGUI.hpp"

#include <DrawCommand.hpp>
#include <log/log.hpp>
#include <MessageType.hpp>
#include <projdefs.h>
#include <queue.h>
#include <Renderer.hpp>
#include <semphr.h>
#include <Service/Bus.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <service-gui/ServiceGUI.hpp>

#include <memory>
#include <utility>
#include <vector>
#include "messages/RenderingFinished.hpp"

namespace sgui
{

    using namespace service::renderer;

    WorkerGUI::WorkerGUI(ServiceGUI *service) : Worker(service)
    {}

    bool WorkerGUI::handleMessage(uint32_t queueID)
    {
        QueueHandle_t queue = queues[queueID];

        auto serviceGUI = static_cast<sgui::ServiceGUI *>(service);

        if (queueID == 0) {
            sys::WorkerCommand received;
            xQueueReceive(queue, &received, 0);

            auto command = static_cast<WorkerGUICommands>(received.command);

            switch (command) {
            case WorkerGUICommands::Render: {
                auto result = serviceGUI->fs.processLastFrame([this](sgui::DrawData &data, ::gui::Context &context) {
                    renderer.render(&context, data.getCommands());
                    return true;
                });

                sys::Bus::SendUnicast(std::make_shared<RenderingFinished>(result), service->GetName(), service);
            } break;

            case WorkerGUICommands::Finish:
                break;
            }
        }
        return true;
    }

} /* namespace sgui */
