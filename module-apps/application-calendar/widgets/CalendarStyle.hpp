// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Style.hpp>
#include <string>

namespace style
{
    namespace window
    {
        namespace calendar
        {
            namespace imageCircleTop
            {
                inline constexpr auto x    = 116U;
                inline constexpr auto y    = 59U;
                inline constexpr auto name = "circle_top";
            } // namespace imageCircleTop
            namespace imageCircleBottom
            {
                inline constexpr auto x    = 106U;
                inline constexpr auto y    = 240U;
                inline constexpr auto name = "circle_bottom";
            } // namespace imageCircleBottom

            namespace name
            {
                inline constexpr auto day_events_window     = "DayEventsWindow";
                inline constexpr auto no_events_window      = "NoEventsWindow";
                inline constexpr auto events_options        = "Options";
                inline constexpr auto dialog_yes_no         = "DialogYesNo";
                inline constexpr auto all_events_window     = "AllEventsWindow";
                inline constexpr auto details_window        = "DetailsWindow";
                inline constexpr auto new_edit_event        = "NewEditEvent";
                inline constexpr auto custom_repeat_window  = "CustomRepeat";
                inline constexpr auto event_reminder_window = "EventReminderWindow";
            } // namespace name

            inline constexpr auto new_event         = "New";
            inline constexpr auto edit_event        = "Edit";
            inline constexpr auto day_cell_width    = 60;
            inline constexpr auto day_cell_height   = 55;
            inline constexpr auto month_year_height = 60;
            inline constexpr auto week_days_number  = 7;
            inline constexpr auto max_weeks_number  = 6;
            inline constexpr auto leftMargin        = 10;

            inline constexpr auto cross_x    = 48;
            inline constexpr auto cross_y    = 55;
            inline constexpr auto arrow_x    = 30;
            inline constexpr auto arrow_y    = 62;
            inline constexpr auto listView_x = style::window::default_left_margin;
            inline constexpr auto listView_y = style::header::height;
            inline constexpr auto listView_w = style::listview::body_width_with_scroll;
            inline constexpr auto listView_h = style::window_height - listView_y - style::footer::height;

            namespace test
            {
                inline constexpr auto prev_month_id = 1;
                inline constexpr auto month_id      = 2;
                inline constexpr auto next_month_id = 3;
                inline constexpr auto date_text_1   = "January 2019";
                inline constexpr auto date_text_2   = "February 2019";
                inline constexpr auto date_text_3   = "March 2019";
            } // namespace test

            namespace item
            {
                namespace dayEvents
                {
                    inline constexpr auto title_w    = 255;
                    inline constexpr auto box_height = style::window::label::small_h;
                    inline constexpr auto height     = 100;
                } // namespace dayEvents

                namespace allEvents
                {
                    inline constexpr auto description_w    = 310;
                    inline constexpr auto start_time_min_w = 60;
                } // namespace allEvents

                namespace repeatAndReminder
                {
                    inline constexpr auto height             = 150;
                    inline constexpr auto title_label_h      = 45;
                    inline constexpr auto title_label_margin = 15;
                    inline constexpr auto description_w      = style::window::default_body_width / 2 - title_label_h;
                    inline constexpr auto description_h      = 30;
                } // namespace repeatAndReminder

                namespace eventDetail
                {
                    inline constexpr auto height_min        = 90;
                    inline constexpr auto height_max        = 155;
                    inline constexpr auto margin_top        = 2 * style::margins::big;
                    inline constexpr auto event_time_margin = 25;
                    inline constexpr auto title_h           = 20;
                    inline constexpr auto label_h           = 35;
                } // namespace eventDetail

                namespace checkBox
                {
                    inline constexpr auto height              = 44;
                    inline constexpr auto margin_top          = 18;
                    inline constexpr auto input_box_label_w   = style::window::label::big_h;
                    inline constexpr auto description_label_w = 280;
                } // namespace checkBox

                namespace severalOptions
                {
                    inline constexpr auto height    = 63;
                    inline constexpr auto label_h   = 20;
                    inline constexpr auto arrow_w_h = 12;
                    inline constexpr auto margin    = 5;
                    inline constexpr auto hBox_h    = height - label_h - margin;
                    inline constexpr auto option_w =
                        style::window::default_body_width - 2 * arrow_w_h - 2 * style::window::calendar::leftMargin;
                } // namespace severalOptions

                namespace textWithLabel
                {
                    inline constexpr auto height        = 63;
                    inline constexpr auto description_h = 20;
                    inline constexpr auto text_input_h  = 37;
                    inline constexpr auto margin        = 6;
                } // namespace textWithLabel
            }     // namespace item
        };        // namespace calendar
    }             // namespace window
} // namespace style
