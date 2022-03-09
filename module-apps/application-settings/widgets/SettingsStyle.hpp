// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace style
{
    namespace settings
    {
        namespace widget
        {
            namespace time
            {
                inline constexpr auto h                   = 107;
                inline constexpr auto description_label_h = 20;
                inline constexpr auto body_h              = h - description_label_h;
                inline constexpr auto separator_w         = 30;
                inline constexpr auto input_w             = 120;

                inline constexpr auto default_time_in_minutes = 720;
                inline constexpr auto max_hour                = 12;
                inline constexpr auto max_minutes             = 59;
                inline constexpr auto max_hour_string         = "12";
                inline constexpr auto max_minutes_string      = "59";
                inline constexpr auto default_input_string    = "00";
                inline constexpr auto before_noon             = "AM";
                inline constexpr auto after_noon              = "PM";
            } // namespace time

            namespace apnInputWidget
            {
                inline constexpr uint32_t w                = style::window::default_body_width;
                inline constexpr uint32_t h                = 63;
                inline constexpr uint32_t title_label_h    = 20;
                inline constexpr uint32_t input_text_h     = 37;
                inline constexpr uint32_t span_size        = 6;
                inline constexpr int32_t underline_padding = 4;
            } // namespace apnInputWidget
        }     // namespace widget

        namespace window
        {
            namespace newApn
            {
                inline constexpr uint32_t x = style::window::default_left_margin;
                inline constexpr uint32_t y = style::window::default_vertical_pos;
                inline constexpr uint32_t w = style::listview::body_width_with_scroll;
                inline constexpr uint32_t h = style::window_height - y - style::nav_bar::height;
            } // namespace newApn


            namespace phone_modes
            {
                inline constexpr auto body_offset = 155U;

                inline constexpr auto description_top_padding      = 34;
                inline constexpr auto description_vertical_padding = 10;
                inline constexpr auto description_y                = 363U;
                inline constexpr auto description_h                = 154U;
            } // namespace phone_modes

            namespace sar
            {
                inline constexpr auto LeftMargin = ::style::window::default_left_margin;
                inline constexpr auto TopMargin =
                    ::style::window::default_vertical_pos + ::style::margins::very_big - 1;
                inline constexpr auto Width  = ::style::window::default_body_width;
                inline constexpr auto Height = ::style::window::default_body_height - ::style::margins::very_big;

                namespace date
                {
                    inline constexpr auto Height = 30;
                } // namespace date
                namespace text
                {
                    inline constexpr auto TopMargin = 7;
                    inline constexpr auto Height    = sar::Height - date::Height - TopMargin;
                } // namespace text
            }     // namespace sar

        } // namespace window
    };    // namespace settings
    namespace quotes::categories
    {
        namespace list
        {
            inline constexpr auto X      = style::window::default_left_margin;
            inline constexpr auto Y      = style::window::default_vertical_pos;
            inline constexpr auto Width  = style::listview::body_width_with_scroll;
            inline constexpr auto Height = style::window_height - Y - style::nav_bar::height;
        } // namespace list

    } // namespace quotes::categories

    namespace certification
    {
        inline constexpr auto entry_with_texts_and_image_min_h = 46;
        inline constexpr auto entry_with_texts_and_image_max_h = 90;
    } // namespace certification

    namespace technical_info
    {
        inline constexpr auto width        = style::window::default_body_width;
        inline constexpr auto height       = 60;
        inline constexpr auto title_height = 27;
        inline constexpr auto value_height = 33;
    } // namespace technical_info
} // namespace style
