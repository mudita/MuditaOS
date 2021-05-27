// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ScreenLightControlParameters.hpp"
#include <memory>
#include <module-sys/Timers/TimerHandle.hpp>
namespace settings
{
    class Settings;
} // namespace settings

namespace sys
{
    class Service;
} // namespace sys

/// Screen light control algorithm. Automatic/Manual mode of operation.
/// Processing of ambient light sensor input to screen brightness output.
namespace screen_light_control
{
    /// Control screen light and keeps it's current state
    class ScreenLightControl
    {
      public:
        explicit ScreenLightControl(sys::Service *parent);
        ~ScreenLightControl();

        void processRequest(Action action);
        void processRequest(Action action, const Parameters &params);

        [[nodiscard]] auto isLightOn() const noexcept -> bool;
        [[nodiscard]] auto getAutoModeState() const noexcept -> ScreenLightMode;
        [[nodiscard]] auto getBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage;

      private:
        void controlTimerCallback();
        void readoutTimerCallback();

        void enableTimers();
        void disableTimers();

        void setParameters(const AutomaticModeParameters &params);
        void setParameters(ManualModeParameters params);
        void setManualBrightnessLevel();

        void turnOff();
        void turnOn();

        void enableAutomaticMode();
        void disableAutomaticMode();

        static constexpr inline auto CONTROL_TIMER_MS = 25;
        static constexpr inline auto READOUT_TIMER_MS = 500;

        sys::TimerHandle controlTimer;
        sys::TimerHandle readoutTimer;

        bool lightOn                                               = false;
        screen_light_control::ScreenLightMode automaticMode        = ScreenLightMode::Manual;
        bsp::eink_frontlight::BrightnessPercentage brightnessValue = 0.0;
    };

} // namespace screen_light_control
