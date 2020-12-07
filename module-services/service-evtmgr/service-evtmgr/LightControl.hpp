// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>
#include <Service/Timer.hpp>
#include <memory>
#include <vector>

namespace sevm::light_control
{
    enum class LightControlAction
    {
        turnOff,
        turnOn,
        enableAutomaticMode,
        disableAutomaticMode
    };

    using BrightnessFunction = std::vector<std::pair<float, float>>;

    typedef struct
    {
        bsp::eink_frontlight::BrightnessPercentage manualModeBrightness = 50;
        BrightnessFunction functionPoints                               = BrightnessFunction({{0.0f, 50.0f}});
        unsigned int rampTimeMS                                         = 1500;
        float brightnessHysteresis                                      = 10.0f;
        float gammaFactor                                               = 2.5f;
    } Parameters;

    void init(sys::Service *parent);

    void deinit();

    bool processRequest(LightControlAction action, const Parameters &params);

    void controlTimerCallback();

    void readoutTimerCallback();

} // namespace sevm::light_control
