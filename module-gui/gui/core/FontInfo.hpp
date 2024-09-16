// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <cstdint>
#include <Common.hpp>
#include <string>

namespace gui
{
    class FontInfo
    {
      public:
        gui::Status load(std::uint8_t *data, std::uint32_t &offset);
        // name of the true type font. max 63 characters
        std::string face;
        // size of the true type font
        std::uint16_t size;
        // flag that informs if font is bold
        std::uint16_t bold;
        // flag that informs if font is italic
        std::uint16_t italic;
        // flag that informs if smoothing was turned on. 1 - smoothing was turned on.
        std::uint16_t smooth;
        // TODO additional space between characters????
        std::int16_t char_spacing;
        // TODO additional space between lines
        std::int16_t line_spacing;
        // distance in pixels between each line of text
        std::uint16_t line_height;
        // number of pixels from the absolute top of the line to the base of the characters
        std::uint16_t base;
        // width of the texture, normally used to scale the x pos of the character image
        std::uint16_t scale_w;
        // height of the texture, normally used to scale the y pos of the character image
        std::uint16_t scale_h;
    };
} // namespace gui
