// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Timer.hpp"

namespace sys
{
    class TimerHandle : public Timer
    {
      public:
        TimerHandle() noexcept = default;
        explicit TimerHandle(Timer *timer) noexcept;
        ~TimerHandle() noexcept override;

        TimerHandle(const TimerHandle &) = delete;
        TimerHandle &operator=(const TimerHandle &) = delete;

        TimerHandle(TimerHandle &&oth) noexcept;
        TimerHandle &operator=(TimerHandle &&oth) noexcept;

        bool isValid() const noexcept;
        void reset(Timer *newTimer = nullptr) noexcept;

        void start() override;
        void restart(std::chrono::milliseconds newInterval) override;
        void stop() override;
        bool isActive() const noexcept override;

      private:
        Timer *timer = nullptr;
    };
} // namespace sys
