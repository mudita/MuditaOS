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
        turnOff,                    ///< Turn off screen frontlight
        turnOn,                     ///< Turn on screen frontlight
        enableAutomaticMode,        ///< Enable automatic mode of screen frontlight
        disableAutomaticMode,       ///< Disable automatic mode of screen frontlight
        setManualModeBrightness,    ///< Set screen brightness in manual mode control
        setAutomaticModeParameters, ///< Set parameters for automatic mode of screen frontlight
        fadeOut                     ///< Set light fade out in automatic mode
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
        functions::BrightnessFunction functionPoints = functions::BrightnessFunction(
            {{0.0f, 35.0f}, {10.0f, 75.0f}, {85.0f, 100.0f}, {500.0f, 100.0f}, {520.0f, 0.0f}});
        /// Ramp time of screen brightness in milliseconds per 0-100% change
        unsigned int rampTimeMS = 1500;
        /// Hysteresis value of screen brightness control
        float brightnessHysteresis = 10.0f;
        /// Gamma factor for screen brightness correction
        float gammaFactor = 2.5f;
    };

    struct LinearProgressModeParameters
    {
        using LinearFunctions = std::vector<functions::LinearProgressFunction>;

        /// Manually set starting value of the brightness
        float startBrightnessValue = 0.0f;
        /// Vector of linear functions of progressive screen brightness
        LinearFunctions functions{
            functions::LinearProgressFunction{.target = 100.0f, .duration = std::chrono::milliseconds{1500}}};
        /// Hysteresis value of screen brightness control
        float brightnessHysteresis = 0.0f;
    };

    struct ConstLinearProgressModeParameters
    {
        float targetBrightness = 0.0f;
    };

    class Parameters
    {
        std::optional<ManualModeParameters> manualModeParams;
        std::optional<AutomaticModeParameters> autoModeParams;
        std::optional<LinearProgressModeParameters> linearProgressModeParams;
        std::optional<ConstLinearProgressModeParameters> constLinearProgressModeParams;

      public:
        Parameters() = default;
        explicit Parameters(ManualModeParameters manualModeParams) : manualModeParams{manualModeParams}
        {}

        explicit Parameters(const AutomaticModeParameters &autoModeParams) : autoModeParams{autoModeParams}
        {}

        explicit Parameters(const LinearProgressModeParameters &autoModeParams)
            : linearProgressModeParams{autoModeParams}
        {}
        explicit Parameters(const ConstLinearProgressModeParameters &autoModeParams)
            : constLinearProgressModeParams{autoModeParams}
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

        [[nodiscard]] bool hasLinearProgressModeParams() const noexcept
        {
            return linearProgressModeParams.has_value();
        }

        [[nodiscard]] auto getLinearProgressModeParams() const noexcept -> const LinearProgressModeParameters &;

        [[nodiscard]] bool hasConstLinearProgressModeParams() const noexcept
        {
            return constLinearProgressModeParams.has_value();
        }

        [[nodiscard]] auto getConstLinearProgressModeParams() const noexcept
            -> const ConstLinearProgressModeParameters &;
    };
} // namespace screen_light_control
