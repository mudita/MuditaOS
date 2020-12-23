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
#include <vector>
#include "messages/RenderingFinished.hpp"

namespace service::gui
{
    namespace
    {
        constexpr auto EnqueueTimeout = 2000 / portTICK_PERIOD_MS;
    } // namespace

    RenderCommand RenderCommand::fromCommandList(std::list<std::unique_ptr<::gui::DrawCommand>> &&commands)
    {
        const auto commandsSize = commands.size();
        auto idx                = 0U;
        auto commandsBuffer     = new ::gui::DrawCommand *[commandsSize];
        for (auto &cmd : commands) {
            commandsBuffer[idx] = cmd.release();
            ++idx;
        }

        RenderCommand renderCommand;
        renderCommand.drawCommands  = commandsBuffer;
        renderCommand.commandsCount = commandsSize;
        return renderCommand;
    }

    void RenderCommand::clear() noexcept
    {
        for (auto i = 0U; i < commandsCount; ++i) {
            delete drawCommands[i];
        }
        commandsCount = 0;

        delete[] drawCommands;
        drawCommands = nullptr;
    }

    auto RenderCommand::toStdList() const -> std::list<::gui::DrawCommand *>
    {
        std::list<::gui::DrawCommand *> commands;
        for (auto i = 0U; i < commandsCount; ++i) {
            commands.emplace_back(drawCommands[i]);
        }
        return commands;
    }

    WorkerGUI::WorkerGUI(ServiceGUI *service) : Worker(service), guiService{service}
    {}

    void WorkerGUI::render(RenderCommand &&command)
    {
        const auto queue = getQueueByName(RenderingQueueName);
        if (!queue->Enqueue(&command, EnqueueTimeout)) {
            LOG_ERROR("Rendering queue full.");
        }
    }

    bool WorkerGUI::handleMessage(uint32_t queueID)
    {
        const auto queue = queues[queueID];
        if (queue->GetQueueName() == RenderingQueueName) {
            if (RenderCommand receivedCommands; queue->Dequeue(&receivedCommands, 0)) {
                auto commandsList = receivedCommands.toStdList();
                render(commandsList, receivedCommands.refreshMode);
                receivedCommands.clear();
            }
        }
        return true;
    }

    void WorkerGUI::render(std::list<::gui::DrawCommand *> &commands, ::gui::RefreshModes refreshMode)
    {
        const auto [contextId, context] = guiService->contextPool->borrowContext(); // Wait for the context.

        renderer.render(context, commands);
        auto msg = std::make_shared<service::gui::RenderingFinished>(contextId, refreshMode);
        sys::Bus::SendUnicast(msg, guiService->GetName(), guiService);
    }
} // namespace service::gui
