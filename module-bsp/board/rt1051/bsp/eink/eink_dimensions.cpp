// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "eink_dimensions.hpp"

#include <eink-config.h>

namespace hal::eink
{

    std::uint32_t getDisplayXAxis(EinkFrame_t frame)
    {
#if defined(EINK_ROTATE_90_CLOCKWISE)
        return BOARD_EINK_DISPLAY_RES_Y - frame.pos_y - frame.height;
#else
        return BOARD_EINK_DISPLAY_RES_X - frame.pos_x - frame.width;
#endif
    }

    std::uint32_t getDisplayYAxis(EinkFrame_t frame)
    {
#if defined(EINK_ROTATE_90_CLOCKWISE)
        return BOARD_EINK_DISPLAY_RES_X - frame.pos_x - frame.width;
#else
        // return BOARD_EINK_DISPLAY_RES_Y - frame.pos_y - frame.height;
        return frame.pos_y;
#endif
    }

    std::uint32_t getDisplayWindowWidth(EinkFrame_t frame)
    {
#if defined(EINK_ROTATE_90_CLOCKWISE)
        return frame.height;
#else
        return frame.width;
#endif
    }

    std::uint32_t getDisplayWindowHeight(EinkFrame_t frame)
    {
#if defined(EINK_ROTATE_90_CLOCKWISE)
        return frame.width;
#else
        return frame.height;
#endif
    }

} // namespace hal::eink
