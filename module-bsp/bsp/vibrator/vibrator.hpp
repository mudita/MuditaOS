#pragma once

#include <Service/Timer.hpp>
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
        void init(sys::ms pulse = static_cast<sys::ms>(defaultVibraPulseMs));
        void deinit();
    } // namespace vibrator
} // namespace bsp
