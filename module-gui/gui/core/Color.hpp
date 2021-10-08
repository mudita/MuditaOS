// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <array>

namespace gui
{
    struct ColorScheme
    {
        static constexpr inline std::uint8_t numberOfColors = 16;
        std::array<std::uint8_t, numberOfColors> intensity;
    };

    struct Color
    {
        constexpr Color() noexcept = default;
        constexpr Color(std::uint8_t intensity, std::uint8_t alpha) noexcept : intensity{intensity}, alpha{alpha}
        {}

        // this defines color of the pixel.
        std::uint8_t intensity{0U};
        // defines how transparent is pixel. 0 means solid color and GUI_COLORS_COUNT-1 fully transparent
        std::uint8_t alpha{0U};

        static constexpr inline std::uint8_t White           = 0x0FU;
        static constexpr inline std::uint8_t Black           = 0x0U;
        static constexpr inline std::uint8_t FullTransparent = 0x0FU;
        static constexpr inline std::uint8_t Opaque          = 0x0U;

        static constexpr inline ColorScheme defaultColorScheme{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    };

    inline bool operator==(const Color &r, const Color &l) noexcept
    {
        return r.intensity == l.intensity && r.alpha == l.alpha;
    }

    inline bool operator==(const ColorScheme &r, const ColorScheme &l) noexcept
    {
        for (int i = 0; i < ColorScheme::numberOfColors; ++i) {
            if (r.intensity[i] != l.intensity[i]) {
                return false;
            }
        }
        return true;
    }

    inline bool operator!=(const Color &r, const Color &l) noexcept
    {
        return !operator==(r, l);
    }

    inline bool operator!=(const ColorScheme &r, const ColorScheme &l) noexcept
    {
        return !operator==(r, l);
    }

    static inline constexpr Color ColorFullBlack{Color::Black, Color::Opaque};
    static inline constexpr Color ColorFullWhite{Color::White, Color::Opaque};
    static inline constexpr Color ColorNoColor{Color::White, Color::FullTransparent};
    static inline constexpr Color ColorGrey{9, Color::Opaque};
    static inline constexpr Color ColorTray{13, Color::Opaque};
} // namespace gui
