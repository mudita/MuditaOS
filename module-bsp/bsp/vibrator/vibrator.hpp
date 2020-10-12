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

        struct Vibration
        {
            std::chrono::milliseconds durationOn  = std::chrono::milliseconds::zero();
            std::chrono::milliseconds durationOff = std::chrono::milliseconds::zero();
        };

        void enable();
        void disable();
        void set(State state);
        void init();
        void deinit();
    } // namespace vibrator
} // namespace bsp
