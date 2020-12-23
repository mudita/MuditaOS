// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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
    /// Interface providing platform-independent synchronization strategy for ContextPool.
    class SynchronizationMechanism
    {
      public:
        using Predicate = std::function<bool()>;

        virtual ~SynchronizationMechanism() noexcept                                                         = default;
        virtual void wait(cpp_freertos::Mutex &lock, std::chrono::milliseconds timeout, Predicate predicate) = 0;
        virtual void notify()                                                                                = 0;
    };
    std::unique_ptr<SynchronizationMechanism> getDefaultContextPoolSynchronizationMechanism();

    class ContextPool
    {
      public:
        ContextPool(::gui::Size screenSize,
                    std::size_t capacity,
                    std::unique_ptr<SynchronizationMechanism> &&synchronization =
                        getDefaultContextPoolSynchronizationMechanism());
        ~ContextPool() noexcept;

        [[nodiscard]] auto peakContext(int id) -> const ::gui::Context *;
        [[nodiscard]] auto borrowContext() noexcept -> std::pair<int, ::gui::Context *>;
        void returnContext(int id) noexcept;

      private:
        [[nodiscard]] auto isAvailable() const noexcept -> bool;

        void freeContext(int contextId);
        void lockContext(int contextId);
        void removeContextId(std::vector<int> &sequence, int contextId);
        void addContextId(std::vector<int> &sequence, int contextId);

        std::vector<std::unique_ptr<::gui::Context>> contexts;
        std::vector<int> lockedContextIds;
        std::vector<int> freeContextIds;

        mutable cpp_freertos::MutexStandard mutex;
        std::unique_ptr<SynchronizationMechanism> synchronization;
    };
} // namespace service::gui
