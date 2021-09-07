#pragma once

#include <module-sys/Timers/Timer.hpp>
#include <Utils.hpp>

#include <chrono>
#include <memory>

namespace bsp
{
    namespace vibrator
    {
        inline constexpr auto defaultVibraPulseMs = 1000;        /// default: 1000 ms vibra pulse
        inline constexpr auto defaultVibraPauseMs = 1000;        /// default: 1000 ms vibra pause between pulses

        enum class Action
        {
            pulse,
            pulseRepeat,
            pulseRepeatInfinite,
            stop,
        };

        void enable();
        void disable();
        void init(std::chrono::milliseconds pulse = std::chrono::milliseconds{defaultVibraPulseMs});
        void deinit();
    } // namespace vibrator
} // namespace bsp
