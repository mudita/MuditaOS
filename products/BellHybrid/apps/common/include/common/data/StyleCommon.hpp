// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <gui/Common.hpp>
#include <Style.hpp>

namespace gui::bell_style
{
    inline constexpr auto font            = ::style::window::font::supersizemelight;
    inline constexpr auto font_center     = ::style::window::font::largelight;
    inline constexpr auto font_status_clock = ::style::window::font::large;

    inline constexpr auto popup_icon_top_margin    = 120;
    inline constexpr auto popup_icon_bottom_margin = 30;

    inline constexpr auto warning_icon_top_margin = 100;
    inline constexpr auto warning_text_width      = 400;

} // namespace gui::bell_style

namespace itStyle
{
    namespace icon
    {
        inline constexpr auto imagePause        = "big_pause";
        inline constexpr auto imageTopMargin    = 112;
        inline constexpr auto imageBottomMargin = 30;
    } // namespace icon

    namespace text
    {
        inline constexpr auto font = ::style::window::font::verybiglight;
    } // namespace text
} // namespace itStyle
