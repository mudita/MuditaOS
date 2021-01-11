// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SynchronizationMechanism.hpp"

#include <gui/core/DrawCommand.hpp>

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

        explicit DrawCommandsQueue(
            std::size_t expectedSize,
            std::unique_ptr<SynchronizationMechanism> &&synchronization = getFreeRtosSynchronizationMechanism());

        void enqueue(QueueItem &&item);
        [[nodiscard]] auto dequeue() -> QueueItem;
        [[nodiscard]] auto getMaxRefreshModeAndClear() -> ::gui::RefreshModes;
        void clear();
        [[nodiscard]] auto size() const noexcept -> QueueContainer::size_type;

      private:
        QueueContainer queue;

        mutable cpp_freertos::MutexStandard queueMutex;
        std::unique_ptr<SynchronizationMechanism> synchronization;
    };
} // namespace service::gui
