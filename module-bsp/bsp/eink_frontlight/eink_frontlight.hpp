// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "bsp/common.hpp"

namespace bsp::eink_frontlight
{
    enum class Action
    {
        turnOn,
        turnOff,
        setBrightness,
    };

    using BrightnessPercentage = float;

    void init();

    void deinit();

    void setBrightness(BrightnessPercentage brightness);

    void turnOn();
    
    void turnOff();

    void setGammaFactor(float gamma);

    void updateClockFrequency(CpuFrequencyMHz newFrequency);
} // namespace bsp::eink_frontlight
