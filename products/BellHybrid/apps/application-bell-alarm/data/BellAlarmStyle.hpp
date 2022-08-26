// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace gui
{
    namespace bell_alarm_style
    {
        namespace time_set_fmt_spinner
        {
            inline constexpr auto focusFont   = style::window::font::supersizeme;
            inline constexpr auto noFocusFont = style::window::font::supersizemelight;
        } // namespace time_set_fmt_spinner

        namespace top_text
        {
            inline constexpr auto font = style::window::font::largelight;
        }

        namespace icon
        {
            constexpr inline auto imageTopMargin    = 112;
            constexpr inline auto imageBottomMargin = 30;
        } // namespace icon
    }     // namespace bell_alarm_style
} // namespace gui
