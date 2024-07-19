// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <cstdint>

#include <eink-config.h>
#include "ED028TC1.hpp"

#if defined(EINK_ROTATE_90_CLOCKWISE)
#define EINK_DISPLAY_RES_X (BOARD_EINK_DISPLAY_RES_Y)
#define EINK_DISPLAY_RES_Y (BOARD_EINK_DISPLAY_RES_X)
#else
#define EINK_DISPLAY_RES_X (BOARD_EINK_DISPLAY_RES_X)
#define EINK_DISPLAY_RES_Y (BOARD_EINK_DISPLAY_RES_Y)
#endif

namespace bsp::eink
{
    std::uint32_t getDisplayXAxis(EinkFrame frame);
    std::uint32_t getDisplayYAxis(EinkFrame frame);
    std::uint32_t getDisplayWindowWidth(EinkFrame frame);
    std::uint32_t getDisplayWindowHeight(EinkFrame frame);
} // namespace bsp::eink
