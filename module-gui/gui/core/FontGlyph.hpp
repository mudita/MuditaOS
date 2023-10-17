// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp" // for Status
#include <stdint.h>   // for uint16_t, uint32_t, uint8_t, int16_t

typedef uint32_t ucode32;

namespace gui
{
    class FontGlyph
    {
      public:
        FontGlyph() = default;
        FontGlyph(const FontGlyph *);
        virtual ~FontGlyph();
        gui::Status load(uint8_t *data, uint32_t &offset);
        gui::Status loadImage(uint8_t *data, uint32_t offset);
        // character id
        ucode32 id = 0;
        // offset in glyph data field
        uint32_t glyph_offset = 0;
        // width of the character image in the texture
        uint16_t width = 0;
        // height of the character image in the texture
        uint16_t height = 0;
        // how much the current position should be offset when copying the image from the texture to the screen
        int16_t xoffset = 0;
        // how much the current position should be offset when copying the image from the texture to the screen
        int16_t yoffset = 0;
        // how much the current position should be advanced after drawing the character
        uint16_t xadvance = 0;
        // image data of the glyph
        uint8_t *data = nullptr;
    };
} // namespace gui
