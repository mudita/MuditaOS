// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>
#include <Service/Timer.hpp>
#include <memory>

namespace sevm::light_control
{
    enum class LightControlAction
    {
        turnOff,
        turnOn,
        enableAutomaticMode,
        disableAutomaticMode,
        setScreenBrightness
    };

    void init(sys::Service *parent);

    void deinit();

    bool processRequest(LightControlAction request, unsigned int value);

    void controlTimerCallback();

    void readoutTimerCallback();

} // namespace sevm::light_control
