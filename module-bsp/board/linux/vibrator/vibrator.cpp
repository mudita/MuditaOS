// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "bsp/vibrator/vibrator.hpp"
#include <log/log.hpp>

namespace bsp::vibrator
{
    void enable()
    {
        LOG_DEBUG("Vibration starts\t\U0001f7e2\U0001f4f3");
    }

    void disable()
    {
        LOG_DEBUG("Vibration ends  \t\U0001f6d1\U0001f4f3");
    }

    void init([[maybe_unused]] std::chrono::milliseconds pulse)
    {}

    void deinit()
    {}

    void updateClockFrequency()
    {}

    void setVibrationLevel([[maybe_unused]] unsigned vibrationLevel)
    {}
} // namespace bsp::vibrator
