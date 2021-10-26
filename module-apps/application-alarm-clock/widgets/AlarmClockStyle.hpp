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
        inline constexpr auto listView_h = style::window_height - listView_y - style::nav_bar::height;

        namespace name
        {
            inline constexpr auto newEditAlarm = "NewEditWindow";
            inline constexpr auto customRepeat = "CustomRepeat";
            inline constexpr auto dialogYesNo  = "DialogYesNo";
        } // namespace name

        namespace item
        {
            inline constexpr auto height         = 100;
            inline constexpr auto botMargin      = 4;
            inline constexpr auto imageMargin    = 150;
            inline constexpr auto timeHeight     = 60 - style::margins::small;
            inline constexpr auto periodHeight   = 40;

            namespace time
            {
                inline constexpr auto width        = style::listview::item_width_with_without_scroll;
                inline constexpr auto height       = 80;
                inline constexpr auto marginTop    = 32;
                inline constexpr auto marginBot    = 20;
                inline constexpr auto separator    = 30;
            } // namespace time

            namespace options
            {
                inline constexpr auto spinner_h = 40;
                inline constexpr auto label_h   = 27;
                inline constexpr auto h         = spinner_h + label_h;
            } // namespace options

        } // namespace item

    } // namespace window
} // namespace style::alarmClock
