// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>
#include "bsp/common.hpp"
#include <utility>

namespace bsp
{
    namespace eink_frontlight
    {
        int32_t init();

        void deinit();

        void setBrightness(uint8_t brightness);

        uint8_t GetBrightness();

        void turnOn();
        
        void turnOff();
    } // namespace eink_frontlight
} // namespace bsp
