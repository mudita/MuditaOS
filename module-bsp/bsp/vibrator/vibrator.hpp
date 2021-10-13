#pragma once

#include <Utils.hpp>
#include <bsp/common.hpp>

#include <chrono>
#include <memory>

namespace bsp
{
    namespace vibrator
    {
        inline constexpr auto defaultVibraPulseMs = 1000; /// default: 1000 ms vibra pulse
        inline constexpr auto defaultVibraPauseMs = 1000; /// default: 1000 ms vibra pause between pulses

        enum class Action
        {
            pulse,
            pulseRepeat,
            pulseRepeatInfinite,
            stop,
        };

        void init(std::chrono::milliseconds pulse = std::chrono::milliseconds{defaultVibraPulseMs});
        void deinit();
        void enable();
        void disable();
        void updateClockFrequency(CpuFrequencyHz newFrequency);
        void setVibrationLevel(unsigned int vibrationLevel);

    } // namespace vibrator
} // namespace bsp
