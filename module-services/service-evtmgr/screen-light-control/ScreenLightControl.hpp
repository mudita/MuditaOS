// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ControlFunctions.hpp"
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
    /// Modes in which front light can operate
    enum ScreenLightMode
    {
        Automatic, /// Automally sets screen brightness based on sensor data
        Manual     /// Manually set brightness level
    };

    /// Set of actions to control the screen light
    enum class Action
    {
        turnOff,             ///< Turn off screen frontlight
        turnOn,              ///< Turn on screen frontlight
        enableAutomaticMode, ///< Enable automatic mode of screen frontlight
        disableAutomaticMode ///< Disable automatic mode of screen frontlight
    };

    /// Set of actions to control the screen light using specified parameters
    enum class ParameterizedAction
    {
        setManualModeBrightness,   ///< Set screen brightness in manual mode control
        setAutomaticModeParameters ///< Set parameters for automatic mode of screen frontlight
    };

    struct Parameters
    {
        static constexpr auto MAX_BRIGHTNESS = 100.0;

        /// Screen brightness 0-100% in manual mode
        bsp::eink_frontlight::BrightnessPercentage manualModeBrightness = 50.0f;
        /// Vector of points for screen brightness [%] in relation to ambient light [Lux] function. Points have to be in
        /// ascending order of ambient light values.
        functions::BrightnessFunction functionPoints = functions::BrightnessFunction({{0.0f, 50.0f}});
        /// Ramp time of screen brightness in miliseconds per 0-100% change
        unsigned int rampTimeMS = 1500;
        /// Hysteresis value of screen brightness control
        float brightnessHysteresis = 10.0f;
        /// Gamma factor for screen brightness correction
        float gammaFactor = 2.5f;
    };

    /// Control screen light and keeps it's current state
    class ScreenLightControl
    {
      public:
        explicit ScreenLightControl(std::shared_ptr<settings::Settings> settings, sys::Service *parent);
        ~ScreenLightControl();

        void processRequest(Action action);
        void processRequest(ParameterizedAction action, Parameters params);

        [[nodiscard]] auto getLightState() const noexcept -> bool;
        [[nodiscard]] auto getAutoModeState() const noexcept -> ScreenLightMode;
        [[nodiscard]] auto getBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage;

      private:
        void controlTimerCallback();
        void readoutTimerCallback();

        void enableTimers();
        void disableTimers();

        void setAutomaticModeParameters(const Parameters &params);
        void setBrightnessLevel(bsp::eink_frontlight::BrightnessPercentage brightnessPercentage);

        void turnOff();
        void turnOn();

        void enableAutomaticMode();
        void disableAutomaticMode();

        void initFromSettings();

        static constexpr inline auto CONTROL_TIMER_MS = 25;
        static constexpr inline auto READOUT_TIMER_MS = 500;

        sys::TimerHandle controlTimer;
        sys::TimerHandle readoutTimer;

        bool lightOn                                               = false;
        screen_light_control::ScreenLightMode automaticMode        = ScreenLightMode::Manual;
        bsp::eink_frontlight::BrightnessPercentage brightnessValue = 0.0;

        std::shared_ptr<settings::Settings> settings;
    };

} // namespace screen_light_control
