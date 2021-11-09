// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/Common.hpp>
#include <Style.hpp>

namespace gui::bell_style
{
    inline constexpr auto font            = ::style::window::font::supersizemelight;
    inline constexpr auto font_center     = ::style::window::font::largelight;
    inline constexpr auto font_top        = ::style::window::font::largelight;
    inline constexpr auto statusClockFont = style::window::font::verybiglight;

    inline constexpr auto popup_icon_top_margin    = 120;
    inline constexpr auto popup_icon_bottom_margin = 30;

} // namespace gui::bell_style
