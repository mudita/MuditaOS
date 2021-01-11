// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SynchronizationMechanism.hpp"

#include <gui/Common.hpp>
#include <gui/core/Context.hpp>

#include <mutex.hpp>

#include <chrono>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace service::gui
{
    class ContextPool
    {
      public:
        ContextPool(
            ::gui::Size screenSize,
            std::size_t capacity,
            std::unique_ptr<SynchronizationMechanism> &&synchronization = getFreeRtosSynchronizationMechanism());

        /**
         * Peeks the context under specific id
         * @param id    Context id
         * @return Context data
         */
        [[nodiscard]] auto peekContext(int id) noexcept -> ::gui::Context *;
        /**
         * Locks the first available context
         * @return A context and its id
         */
        [[nodiscard]] auto borrowContext() -> std::pair<int, ::gui::Context *>;
        /**
         * Locks the context
         * @param id    Context id
         * @return Context data
         */
        [[nodiscard]] auto borrowContext(int id) -> ::gui::Context *;
        /**
         * Checks whether any context is currently locked
         * @return True if any context is locked, false otherwise
         */
        [[nodiscard]] auto isAnyContextLocked() const -> bool;
        /**
         * Makes a context (specified by its id) available again.
         * @param id    Context id
         */
        void returnContext(int id);

      private:
        using ContextIdContainer = std::vector<int>;

        [[nodiscard]] auto isAvailable() const noexcept -> bool;

        void freeContext(int contextId);
        void lockContext(int contextId);
        static void removeContextId(std::vector<int> &sequence, int contextId);
        static void addContextId(std::vector<int> &sequence, int contextId);

        std::vector<std::unique_ptr<::gui::Context>> contexts;
        ContextIdContainer lockedContextIds;
        ContextIdContainer freeContextIds;

        mutable cpp_freertos::MutexStandard mutex;
        std::unique_ptr<SynchronizationMechanism> synchronization;
    };
} // namespace service::gui
