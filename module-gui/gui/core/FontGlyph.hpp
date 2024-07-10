// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"
#include <cstdint>

using ucode32 = std::uint32_t;

namespace gui
{
    class FontGlyph
    {
      public:
        FontGlyph() = default;
        explicit FontGlyph(const FontGlyph *from);

        virtual ~FontGlyph();

        gui::Status load(std::uint8_t *data, std::uint32_t &offset);
        gui::Status loadImage(std::uint8_t *data, std::uint32_t offset);

        // Character id
        ucode32 id = 0;
        // Offset in glyph data field
        std::uint32_t glyph_offset = 0;
        // Width of the character image in the texture
        std::uint16_t width = 0;
        // Height of the character image in the texture
        std::uint16_t height = 0;
        // How much the current position should be offset when copying the image from the texture to the screen
        std::int16_t xoffset = 0;
        // How much the current position should be offset when copying the image from the texture to the screen
        std::int16_t yoffset = 0;
        // How much the current position should be advanced after drawing the character
        std::uint16_t xadvance = 0;
        // Image data of the glyph
        std::uint8_t *data = nullptr;
    };
} // namespace gui
