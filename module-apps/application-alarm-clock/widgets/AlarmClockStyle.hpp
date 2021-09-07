// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace style::alarmClock
{
    inline constexpr auto editAlarm = "Edit";
    inline constexpr auto newAlarm  = "New";

    namespace window
    {
        inline constexpr auto listView_x = style::window::default_left_margin;
        inline constexpr auto listView_y = style::window::default_vertical_pos;
        inline constexpr auto listView_w = style::listview::body_width_with_scroll;
        inline constexpr auto listView_h = style::window_height - listView_y - style::footer::height;

        namespace name
        {
            inline constexpr auto newEditAlarm = "NewEditWindow";
            inline constexpr auto customRepeat = "CustomRepeat";
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

            namespace time
            {
                inline constexpr auto height       = 106;
                inline constexpr auto margin       = 21;
                inline constexpr auto marginBot    = 15;
                inline constexpr auto separator    = 30;
                inline constexpr auto timeInput12h = 120;
                inline constexpr auto timeInput24h = 195;
            } // namespace time

            namespace options
            {
                inline constexpr auto height    = 63;
                inline constexpr auto label_h   = 30;
                inline constexpr auto arrow_w_h = 20;
            } // namespace options

            namespace checkBox
            {
                inline constexpr auto height        = 44;
                inline constexpr auto marginTop     = 18;
                inline constexpr auto inputBox_w    = style::window::label::big_h;
                inline constexpr auto description_w = 280;
            } // namespace checkBox
        } // namespace item

    } // namespace window
} // namespace style::alarmClock
