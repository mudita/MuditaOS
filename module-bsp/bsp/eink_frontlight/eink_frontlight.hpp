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

    using BrightnessLevel = uint8_t;

    void init();

    void deinit();

    void setBrightness(BrightnessLevel brightness); // In 1-5 scale

    void turnOn();
    
    void turnOff();
} // namespace bsp::eink_frontlight
