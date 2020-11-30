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

    void init();

    void deinit();

    void setBrightness(std::uint8_t brightness);

    std::uint8_t GetBrightness();

    void turnOn();
    
    void turnOff();
} // namespace bsp::eink_frontlight
