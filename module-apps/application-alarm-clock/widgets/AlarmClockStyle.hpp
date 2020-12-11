// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace style::alarmClock
{
    namespace window
    {
        inline constexpr auto cross_x    = 48;
        inline constexpr auto cross_y    = 55;
        inline constexpr auto arrow_x    = 30;
        inline constexpr auto arrow_y    = 62;
        inline constexpr auto listView_x = style::window::default_left_margin;
        inline constexpr auto listView_y = style::header::height;
        inline constexpr auto listView_w = style::listview::body_width_with_scroll;
        inline constexpr auto listView_h = style::window_height - listView_y - style::footer::height;

        namespace name
        {
            inline constexpr auto dialogYesNo = "DialogYesNo";
        }

        namespace item
        {
            inline constexpr auto height         = 100;
            inline constexpr auto vBoxWidth      = 200;
            inline constexpr auto botMargin      = 4;
            inline constexpr auto vBoxLeftMargin = 10;
            inline constexpr auto imageMargin    = 150;
            inline constexpr auto timeHeight     = 60 - style::margins::small;
            inline constexpr auto periodHeight   = 40;
        } // namespace item

    } // namespace window
} // namespace style::alarmClock
