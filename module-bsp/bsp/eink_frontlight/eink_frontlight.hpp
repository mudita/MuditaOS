// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "bsp/common.hpp"

namespace bsp::eink_frontlight
{
    enum class Action
    {
        TurnOn,
        TurnOff,
        SetBrightness,
    };

    using BrightnessPercentage = float;

    void init();
    void deinit();

    void turnOn();
    void turnOff();

    void setBrightness(BrightnessPercentage brightness);
    void setGammaFactor(float gamma);

    void updateClockFrequency();
}
