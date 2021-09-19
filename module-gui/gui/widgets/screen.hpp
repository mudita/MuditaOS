// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

//
// Created by pawelp on 13.05.2021.
//
#pragma once

#include <board/board.h>

constexpr unsigned getScreenResolutionX()
{
    return BOARD_EINK_DISPLAY_RES_X;
}

constexpr unsigned getScreenResolutionY()
{
    return BOARD_EINK_DISPLAY_RES_Y;
}
