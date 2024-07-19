// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkDimensions.hpp"
#include <eink-config.h>

namespace bsp::eink
{
    std::uint32_t getDisplayXAxis(EinkFrame frame)
    {
#if defined(EINK_ROTATE_90_CLOCKWISE)
        return BOARD_EINK_DISPLAY_RES_Y - frame.posY - frame.height;
#else
        return BOARD_EINK_DISPLAY_RES_X - frame.posX - frame.width;
#endif
    }

    std::uint32_t getDisplayYAxis(EinkFrame frame)
    {
#if defined(EINK_ROTATE_90_CLOCKWISE)
        return BOARD_EINK_DISPLAY_RES_X - frame.posX - frame.width;
#else
        return frame.posY;
#endif
    }

    std::uint32_t getDisplayWindowWidth(EinkFrame frame)
    {
#if defined(EINK_ROTATE_90_CLOCKWISE)
        return frame.height;
#else
        return frame.width;
#endif
    }

    std::uint32_t getDisplayWindowHeight(EinkFrame frame)
    {
#if defined(EINK_ROTATE_90_CLOCKWISE)
        return frame.width;
#else
        return frame.height;
#endif
    }
} // namespace bsp::eink
