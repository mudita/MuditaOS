#pragma once
#include <Style.hpp>
#include <utf8/UTF8.hpp>

namespace musicPlayerStyle
{
    namespace mainWindow
    {
        constexpr uint32_t x = style::window::default_left_margin;
        // Magic 1 -> discussed with Design for proper alignment.
        constexpr uint32_t y = style::header::height - 1;
        constexpr uint32_t w = style::listview::body_width_with_scroll;
        // Bottom margin need to be added to fit all elements.
        constexpr uint32_t h = style::window_height - y - style::footer::height + style::margins::small;
    } // namespace mainWindow

    namespace songItem
    {
        constexpr uint32_t w = style::window::default_body_width;
        constexpr uint32_t h = 100;

        constexpr uint32_t text_h     = 22;
        constexpr uint32_t duration_w = 50;

        constexpr int32_t topMargin   = 18;
        constexpr int32_t leftMargin  = 10;
        constexpr int32_t rightMargin = 10;

    } // namespace songItem

} // namespace musicPlayerStyle
