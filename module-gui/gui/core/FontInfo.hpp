// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>   // for uint16_t, uint32_t, uint8_t
#include <Common.hpp> // for Status
#include <string>     // for string

namespace gui
{
    class FontInfo
    {
      public:
        gui::Status load(uint8_t *data, uint32_t &offset);
        // name of the true type font. max 63 characters
        std::string face;
        // size of the true type font
        uint16_t size;
        // flag that informs if font is bold
        uint16_t bold;
        // flag that informs if font is italic
        uint16_t italic;
        // flag that informs if smoothing was turned on. 1 - smoothing was turned on.
        uint16_t smooth;
        // TODO additional space between characters????
        uint16_t char_spacing;
        // TODO additional space between lines
        uint16_t line_spacing;
        // distance in pixels between each line of text
        uint16_t line_height;
        // number of pixels from the absolute top of the line to the base of the characters
        uint16_t base;
        // width of the texture, normally used to scale the x pos of the character image
        uint16_t scale_w;
        // height of the texture, normally used to scale the y pos of the character image
        uint16_t scale_h;
    };
} // namespace gui
