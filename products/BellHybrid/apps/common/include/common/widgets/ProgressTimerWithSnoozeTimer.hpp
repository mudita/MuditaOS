// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <apps-common/widgets/ProgressTimer.hpp>
#include <Timers/TimerHandle.hpp>
#include <time/time_conversion.hpp>
#include <atomic>
#include <chrono>
#include <string>

namespace gui
{
    class SnoozeTimer;
} // namespace gui

namespace app
{
    class ProgressTimerWithSnoozeTimer : public ProgressTimer
    {
        gui::SnoozeTimer *timer = nullptr;

        void update() override;
        void updateTime();

      public:
        using ProgressTimer::ProgressTimer;

        void attach(gui::SnoozeTimer *time);
    };

} // namespace app
