// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/core/DrawCommand.hpp>
#include <mutex.hpp>

#include <cstdint>
#include <list>
#include <memory>
#include <vector>

namespace service::gui
{
    class DrawCommandsQueue
    {
      public:
        using CommandList = std::list<std::unique_ptr<::gui::DrawCommand>>;
        struct QueueItem
        {
            CommandList commands;
            ::gui::RefreshModes refreshMode = ::gui::RefreshModes::GUI_REFRESH_FAST;
        };
        using QueueContainer = std::vector<QueueItem>;

        explicit DrawCommandsQueue(std::size_t expectedSize);

        void enqueue(QueueItem &&item);
        auto stop() -> void;
        [[nodiscard]] auto dequeue() -> std::optional<QueueItem>;
        [[nodiscard]] auto getMaxRefreshModeAndClear() -> ::gui::RefreshModes;
        void clear();
        [[nodiscard]] auto size() const noexcept -> QueueContainer::size_type;

      private:
        QueueContainer queue;

        mutable cpp_freertos::MutexStandard queueMutex;
    };
} // namespace service::gui
