// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include "bsp/common.hpp"
#include <utility>

namespace bsp::eink_frontlight
{
    enum class Action
    {
        turnOn,
        turnOff,
        setBrightness,
    };

    using BrightnessPercentage = uint8_t;

    void init();

    void deinit();

    void setBrightness(BrightnessPercentage brightness);

    void turnOn();
    
    void turnOff();

    void setGammaFactor(float gamma);
} // namespace bsp::eink_frontlight
