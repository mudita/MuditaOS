#pragma once

#include <cstdint>

namespace gui
{
    struct Color
    {
        constexpr Color(uint8_t intensity = 0, uint8_t alpha = 0) : intensity{intensity}, alpha{alpha} {};
        // this defines color of the pixel.
        uint8_t intensity = 0;
        // defines how transparent is pixel. 0 means solid color and GUI_COLORS_COUNT-1 fully transparent
        uint8_t alpha = 0;
    };

    extern Color ColorFullBlack;
    extern Color ColorFullWhite;
    extern Color ColorNoColor;
    extern Color ColorTray;

    inline bool operator==(const Color &r, const Color &l)
    {
        return r.intensity == l.intensity && r.alpha == l.alpha;
    }

    inline bool operator!=(const Color &r, const Color &l)
    {
        return !(r == l);
    }
} // namespace gui
