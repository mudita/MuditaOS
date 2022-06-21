// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

#include <eink-config.h>
#include "ED028TC1.h"

#if defined(EINK_ROTATE_90_CLOCKWISE)
#define EINK_DISPLAY_RES_X (BOARD_EINK_DISPLAY_RES_Y)
#define EINK_DISPLAY_RES_Y (BOARD_EINK_DISPLAY_RES_X)
#else
#define EINK_DISPLAY_RES_X (BOARD_EINK_DISPLAY_RES_X)
#define EINK_DISPLAY_RES_Y (BOARD_EINK_DISPLAY_RES_Y)
#endif

namespace hal::eink
{
    std::uint32_t getDisplayXAxis(EinkFrame_t frame);
    std::uint32_t getDisplayYAxis(EinkFrame_t frame);
    std::uint32_t getDisplayWindowWidth(EinkFrame_t frame);
    std::uint32_t getDisplayWindowHeight(EinkFrame_t frame);
} // namespace hal::eink
