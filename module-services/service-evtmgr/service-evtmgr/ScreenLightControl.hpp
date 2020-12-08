// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>
#include <Service/Timer.hpp>
#include <memory>
#include <vector>

namespace sevm::screen_light_control
{
    /// Set of actions to control the screen light
    enum class Action
    {
        turnOff,                    ///< Turn off screen frontlight
        turnOn,                     ///< Turn on screen frontlight
        enableAutomaticMode,        ///< Enable automatic mode of screen frontlight
        disableAutomaticMode,       ///< Disable automatic mode of screen frontlight
        setManualModeBrightness,    ///< Set screen brightness in manual mode control
        setGammaCorrectionFactor,   ///< Set gamma factor for screen frontlight correction
        setAutomaticModeParameters, ///< Set parameters for automatic mode of screen frontlight
    };

    using BrightnessFunction =
        std::vector<std::pair<bsp::light_sensor::IlluminanceLux, bsp::eink_frontlight::BrightnessPercentage>>;

    typedef struct
    {
        /// Screen brightness 0-100% in manual mode
        bsp::eink_frontlight::BrightnessPercentage manualModeBrightness = 50.0f;
        /// Vector of points for screen brightness [%] in relation to ambient light [Lux] function. Points have to be in
        /// ascending order of ambient light values.
        BrightnessFunction functionPoints = BrightnessFunction({{0.0f, 50.0f}});
        /// Ramp time of screen brightness in miliseconds per 0-100% change
        unsigned int rampTimeMS = 1500;
        /// Hysteresis value of screen brightness control
        float brightnessHysteresis = 10.0f;
        /// Gamma factor for screen brightness correction
        float gammaFactor = 2.5f;
    } Parameters;

    /// Initialization of screen light control
    /// @param 'parent' - pointer to parent sys::Service class
    void init(sys::Service *parent);

    void deinit();

    void processRequest(Action action, const Parameters &params);

} // namespace sevm::screen_light_control
