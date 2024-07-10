// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontInfo.hpp"
#include "Common.hpp"
#include <cstring>

namespace gui
{
    gui::Status FontInfo::load(std::uint8_t *data, std::uint32_t &offset)
    {
        constexpr auto maxFontNameLength = 63;                    // Up to 63 chars of the font's name
        constexpr auto fontNameSize      = maxFontNameLength + 1; // Include null-terminator

        char name[fontNameSize] = {0};
        std::memcpy(name, &data[offset], maxFontNameLength);
        offset += fontNameSize;
        face = std::string(name);

        // Size of the true type font
        std::memcpy(&size, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // Flag that informs if font is bold
        std::memcpy(&bold, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // Flag that informs if font is italic
        std::memcpy(&italic, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // Flag that informs if smoothing was turned on. 1 - smoothing was turned on.
        std::memcpy(&smooth, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // TODO additional space between characters?
        std::memcpy(&char_spacing, &data[offset], sizeof(std::int16_t));
        offset += sizeof(std::int16_t);

        // TODO additional space between lines
        std::memcpy(&line_spacing, &data[offset], sizeof(std::int16_t));
        offset += sizeof(std::int16_t);

        // Distance in pixels between each line of text
        std::memcpy(&line_height, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // Number of pixels from the absolute top of the line to the base of the characters
        std::memcpy(&base, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // Width of the texture, normally used to scale the x pos of the character image
        std::memcpy(&scale_w, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // Height of the texture, normally used to scale the y pos of the character image
        std::memcpy(&scale_h, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        return gui::Status::GUI_SUCCESS;
    }
} // namespace gui
