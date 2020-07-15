#pragma once

#include <stdint.h>   // for uint16_t, int16_t, uint32_t, uint8_t
#include <Common.hpp> // for Status

namespace gui
{
    class FontKerning
    {
      public:
        gui::Status load(uint8_t *data, uint32_t &offset);
        // utf16 id of the first character
        uint16_t first;
        // utf16 id of the following character
        uint16_t second;
        // distance in pixels between beginning of first character and beginning of second character
        int16_t amount;
    };
} // namespace gui
