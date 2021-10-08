// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>

#include <chrono>
#include <vector>
#include <utility>

namespace screen_light_control::functions
{
    using BrightnessFunction =
        std::vector<std::pair<bsp::light_sensor::IlluminanceLux, bsp::eink_frontlight::BrightnessPercentage>>;

    struct LinearProgressFunction
    {
        float target                       = 100.0f;
        std::chrono::milliseconds duration = std::chrono::milliseconds::zero();
    };

    bsp::eink_frontlight::BrightnessPercentage brightnessRampOut();

    void calculateBrightness(bsp::light_sensor::IlluminanceLux measurement);

    void setRampState(float state);

    void setRampStep(float step);

    void setHysteresis(float hyst);

    void setFunctionFromPoints(const BrightnessFunction &points);

    void setRampTarget(bsp::eink_frontlight::BrightnessPercentage value);

    bool isRampTargetReached();

    bsp::eink_frontlight::BrightnessPercentage getRampState();
} // namespace screen_light_control::functions
