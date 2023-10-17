// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <mutex.hpp>

#include <chrono>
#include <functional>
#include <memory>

namespace service::gui
{
    /// Interface providing platform-independent synchronization strategy.
    class SynchronizationMechanism
    {
      public:
        using Predicate = std::function<bool()>;

        virtual ~SynchronizationMechanism() noexcept                                                         = default;
        virtual void wait(cpp_freertos::Mutex &lock, std::chrono::milliseconds timeout, Predicate predicate) = 0;
        virtual void notify()                                                                                = 0;
    };

    std::unique_ptr<SynchronizationMechanism> getFreeRtosSynchronizationMechanism();
} // namespace service::gui
