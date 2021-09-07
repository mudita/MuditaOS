// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ControlFunctions.hpp"
#include <optional>

namespace screen_light_control
{
    /// Modes in which front light can operate
    enum class ScreenLightMode
    {
        Automatic, /// Automatically sets screen brightness based on sensor data
        Manual     /// Manually set brightness level
    };

    /// Set of actions to control the screen light
    enum class Action
    {
        turnOff,                   ///< Turn off screen frontlight
        turnOn,                    ///< Turn on screen frontlight
        enableAutomaticMode,       ///< Enable automatic mode of screen frontlight
        disableAutomaticMode,      ///< Disable automatic mode of screen frontlight
        setManualModeBrightness,   ///< Set screen brightness in manual mode control
        setAutomaticModeParameters ///< Set parameters for automatic mode of screen frontlight
    };

    struct ManualModeParameters
    {
        static constexpr auto MAX_BRIGHTNESS = 100.0;
        /// Screen brightness 0-100% in manual mode
        bsp::eink_frontlight::BrightnessPercentage manualModeBrightness = 50.0f;
    };

    struct AutomaticModeParameters
    {
        /// Vector of points for screen brightness [%] in relation to ambient light [Lux] function. Points have to be in
        /// ascending order of ambient light values.
        functions::BrightnessFunction functionPoints =
            functions::BrightnessFunction({{0.0f, 70.0f}, {250.0f, 70.0f}, {450.0f, 40.0f}, {500.0f, 0.0f}});
        /// Ramp time of screen brightness in milliseconds per 0-100% change
        unsigned int rampTimeMS = 1500;
        /// Hysteresis value of screen brightness control
        float brightnessHysteresis = 10.0f;
        /// Gamma factor for screen brightness correction
        float gammaFactor = 2.5f;
    };

    class Parameters
    {
        std::optional<ManualModeParameters> manualModeParams;
        std::optional<AutomaticModeParameters> autoModeParams;

      public:
        Parameters() = default;
        explicit Parameters(ManualModeParameters manualModeParams)
            : manualModeParams{std::make_optional(manualModeParams)}
        {}
        explicit Parameters(const AutomaticModeParameters &autoModeParams)
            : autoModeParams{std::make_optional(autoModeParams)}
        {}

        [[nodiscard]] bool hasManualModeParams() const noexcept
        {
            return manualModeParams.has_value();
        }

        [[nodiscard]] auto getManualModeParams() const noexcept -> ManualModeParameters;

        [[nodiscard]] bool hasAutoModeParams() const noexcept
        {
            return autoModeParams.has_value();
        }

        [[nodiscard]] auto getAutoModeParams() const noexcept -> const AutomaticModeParameters &;
    };
} // namespace screen_light_control
