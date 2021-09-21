// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerGUI.hpp"

#include <DrawCommand.hpp>
#include <log/log.hpp>
#include <Renderer.hpp>
#include <Service/Worker.hpp>
#include <service-gui/ServiceGUI.hpp>

#include <memory>
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
        switch (command) {
        case Signal::Render: {
            auto item = guiService->commandsQueue->dequeue();
            render(item.commands, item.refreshMode);
            break;
        }
        case Signal::ChangeColorScheme: {
            changeColorScheme(guiService->colorSchemeUpdate);
            break;
        }
        default:
            LOG_ERROR("Command not valid.");
        }
    }

    void WorkerGUI::render(DrawCommandsQueue::CommandList &commands, ::gui::RefreshModes refreshMode)
    {
        const auto [contextId, context] = guiService->contextPool->borrowContext(); // Waits for the context.
        renderer.render(context, commands);
        onRenderingFinished(contextId, refreshMode);
    }

    void WorkerGUI::changeColorScheme(const std::unique_ptr<::gui::ColorScheme> &scheme)
    {
        renderer.changeColorScheme(scheme);
    }

    void WorkerGUI::onRenderingFinished(int contextId, ::gui::RefreshModes refreshMode)
    {
        auto msg = std::make_shared<service::gui::RenderingFinished>(contextId, refreshMode);
        guiService->bus.sendUnicast(std::move(msg), guiService->GetName());
    }
} // namespace service::gui
