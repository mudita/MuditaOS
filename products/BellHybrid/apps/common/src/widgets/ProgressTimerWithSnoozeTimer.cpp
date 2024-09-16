// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <common/widgets/ProgressTimerWithSnoozeTimer.hpp>
#include <common/widgets/SnoozeTimer.hpp>
#include <ApplicationCommon.hpp>
#include <apps-common/GuiTimer.hpp>
#include <gsl/assert>

namespace
{
    constexpr auto increasingModePrefix = "-";
}

namespace app
{
    void ProgressTimerWithSnoozeTimer::update()
    {
        updateTime();
        ProgressTimer::update();
    }

    void ProgressTimerWithSnoozeTimer::updateTime()
    {
        using utils::time::Duration;
        if (timer == nullptr) {
            return;
        }
        const auto secondsRemaining = duration - elapsed;
        const Duration remainingDuration{std::time_t{secondsRemaining.count()}};

        timer->setTime(remainingDuration.getMinutes(), remainingDuration.getSeconds());
    }

    void ProgressTimerWithSnoozeTimer::attach(gui::SnoozeTimer *_timer)
    {
        Expects(_timer != nullptr);
        timer = _timer;
    }
} // namespace app
