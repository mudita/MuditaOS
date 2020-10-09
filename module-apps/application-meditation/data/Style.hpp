#pragma once

namespace style::meditation
{
    namespace body
    {
        constexpr auto Width  = style::window_width;
        constexpr auto Height = style::window_height - style::header::height - style::footer::height - 1;
        constexpr auto X      = 1;
        constexpr auto Y      = style::header::height;
    } // namespace body

    namespace timer
    {
        constexpr auto Radius = 150;
        constexpr auto X      = body::X + (body::Width / 2) - Radius;
        constexpr auto Y      = body::Y + (body::Height / 2) - Radius;
        constexpr auto Width  = 2 * Radius;
        constexpr auto Height = Width;

        constexpr auto BorderColor = gui::ColorGrey;
        constexpr auto PenWidth    = 3;
    } // namespace timer
} // namespace style::meditation
