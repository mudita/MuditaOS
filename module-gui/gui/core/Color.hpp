#pragma once

#include <cstdint>

namespace gui
{
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
    };

    inline bool operator==(const Color &r, const Color &l) noexcept
    {
        return r.intensity == l.intensity && r.alpha == l.alpha;
    }

    inline bool operator!=(const Color &r, const Color &l) noexcept
    {
        return !operator==(r, l);
    }

    static inline constexpr Color ColorFullBlack{Color::Black, Color::Opaque};
    static inline constexpr Color ColorFullWhite{Color::White, Color::Opaque};
    static inline constexpr Color ColorNoColor{Color::White, Color::FullTransparent};
    static inline constexpr Color ColorTray{13, Color::Opaque};
} // namespace gui
