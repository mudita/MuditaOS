// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>   // for uint16_t, int16_t, uint32_t, uint8_t
#include <Common.hpp> // for Status

typedef uint32_t ucode32;

namespace gui
{
    class FontKerning
    {
      public:
        gui::Status load(uint8_t *data, uint32_t &offset);
        // utf16 id of the first character
        ucode32 first;
        // utf16 id of the following character
        ucode32 second;
        // distance in pixels between beginning of first character and beginning of second character
        int16_t amount;
    };
} // namespace gui
