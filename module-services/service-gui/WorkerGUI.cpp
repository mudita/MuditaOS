// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerGUI.hpp"

#include <DrawCommand.hpp>
#include <log/log.hpp>
#include <Renderer.hpp>
#include <Service/Bus.hpp>
#include <Service/Worker.hpp>
#include <service-gui/ServiceGUI.hpp>

#include <memory>
#include <module-utils/time/ScopedTime.hpp>
#include "messages/RenderingFinished.hpp"

namespace service::gui
{
    WorkerGUI::WorkerGUI(ServiceGUI *service) : Worker(service), guiService{service}
    {}

    void WorkerGUI::notify(Signal command)
    {
        if (auto queue = getQueueByName(SignallingQueueName); !queue->Overwrite(&command)) {
            LOG_ERROR("Unable to overwrite the command in the commands queue.");
        }
    }

    bool WorkerGUI::handleMessage(uint32_t queueID)
    {
        if (const auto queue = queues[queueID]; queue->GetQueueName() == SignallingQueueName) {
            if (sys::WorkerCommand command; queue->Dequeue(&command, 0)) {
                handleCommand(static_cast<Signal>(command.command));
            }
        }
        return true;
    }

    void WorkerGUI::handleCommand(Signal command)
    {
        if (command == Signal::Render) {
            auto item = guiService->commandsQueue->dequeue();
            render(item.commands, item.refreshMode);
        }
    }

    void WorkerGUI::render(std::list<std::unique_ptr<::gui::DrawCommand>> &commands, ::gui::RefreshModes refreshMode)
    {
        const auto [contextId, context] = guiService->contextPool->borrowContext(); // Waits for the context.
        renderer.render(context, commands);
        onRenderingFinished(contextId, refreshMode);
    }

    void WorkerGUI::onRenderingFinished(int contextId, ::gui::RefreshModes refreshMode)
    {
        auto msg = std::make_shared<service::gui::RenderingFinished>(contextId, refreshMode);
        sys::Bus::SendUnicast(std::move(msg), guiService->GetName(), guiService);
    }
} // namespace service::gui
