// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceGUI.hpp"

#include <core/Context.hpp>
#include <core/DrawCommand.hpp>
#include <gui/core/Renderer.hpp>
#include <Service/Worker.hpp>

#include <cstdint>

namespace service::gui
{
    class ServiceGUI;

    struct RenderCommand
    {
        ::gui::DrawCommand **drawCommands = nullptr;
        std::size_t commandsCount         = 0U;
        ::gui::RefreshModes refreshMode   = ::gui::RefreshModes::GUI_REFRESH_FAST;

        static RenderCommand fromCommandList(std::list<std::unique_ptr<::gui::DrawCommand>> &&commands);

        void clear() noexcept;
        [[nodiscard]] auto toStdList() const -> std::list<::gui::DrawCommand *>;
    };

    class WorkerGUI : public sys::Worker
    {
      public:
        static constexpr auto RenderingQueueName = "RendererQueue";

        explicit WorkerGUI(ServiceGUI *service);

        auto handleMessage(std::uint32_t queueID) -> bool override;
        void render(RenderCommand &&command);

      private:
        void render(std::list<::gui::DrawCommand *> &commands, ::gui::RefreshModes refreshMode);

        ServiceGUI *guiService;
        ::gui::Renderer renderer;
    };
} // namespace service::gui
