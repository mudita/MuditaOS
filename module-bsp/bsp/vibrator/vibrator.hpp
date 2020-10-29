#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#include <chrono>

namespace bsp
{
    namespace vibrator
    {
        enum class State : bool
        {
            On,
            Off,
        };

        /// A pulse of vibration
        struct VibrationPulse
        {
            /// for how long the vibration will be on
            std::chrono::milliseconds pulse;
            /// optional recurrence of pulse
            std::chrono::milliseconds period = std::chrono::milliseconds::zero();
        };

        void enable();
        void disable();
        void set(State state);
        void init();
        void deinit();
    } // namespace vibrator
} // namespace bsp
