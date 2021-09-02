// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/Style.hpp>

#include <string>

namespace style::window::calendar
{
    inline constexpr auto day_cell_width    = 60;
    inline constexpr auto day_cell_height   = 55;
    inline constexpr auto month_year_height = 60;
    inline constexpr auto week_days_number  = 7;

    inline constexpr auto listView_x = style::window::default_left_margin;
    inline constexpr auto listView_y = style::window::default_vertical_pos;
    inline constexpr auto listView_w = style::listview::body_width_with_scroll;
    inline constexpr auto listView_h = style::window_height - listView_y - style::footer::height;

    namespace item
    {
        namespace dayEvents
        {
            inline constexpr auto title_w    = 255;
            inline constexpr auto box_height = style::window::label::small_h;
            inline constexpr auto height     = 100;
        } // namespace dayEvents
    }     // namespace item
} // namespace style::window::calendar
