// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <functional>

namespace app
{

    class TimerWithCallbacks
    {
      public:
        virtual ~TimerWithCallbacks()                                                    = default;
        [[nodiscard]] virtual auto isStopped() const noexcept -> bool                    = 0;
        virtual void reset(std::chrono::seconds duration,
                           std::chrono::seconds interval = std::chrono::seconds::zero()) = 0;
        virtual void start()                                                             = 0;
        virtual void stop()                                                              = 0;
        virtual void registerOnFinishedCallback(std::function<void()> cb)                = 0;
        virtual void registerOnIntervalCallback(std::function<void()> cb)                = 0;
    };

} // namespace app
