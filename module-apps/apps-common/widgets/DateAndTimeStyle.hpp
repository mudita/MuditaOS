// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace style::window::date_and_time
{
    inline constexpr auto leftMargin       = 10;
    inline constexpr auto height           = 107;
    inline constexpr auto topMargin        = 26;
    inline constexpr auto separator        = 30;
    inline constexpr auto time_input_12h_w = 120;
    inline constexpr auto time_input_24h_w = 195;
    inline constexpr auto hBox_h           = height - 1.25 * topMargin;
    inline constexpr auto listView_x       = style::window::default_left_margin;
    inline constexpr auto listView_y       = style::window::default_vertical_pos;
    inline constexpr auto listView_w       = style::listview::body_width_with_scroll;
    inline constexpr auto listView_h       = style::window_height - listView_y - style::footer::height;
} // namespace style::window::date_and_time
