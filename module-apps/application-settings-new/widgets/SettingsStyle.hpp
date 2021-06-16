// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Style.hpp>
#include <string>

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
            namespace languageChange
            {
                inline constexpr auto options_posX             = 17;
                inline constexpr auto options_posY             = 100;
                inline constexpr auto options_distance_between = 60;
            } // namespace languageChange

            namespace torch
            {
                inline constexpr auto body_offset = 155;

                inline constexpr auto bar_y = 396;
                inline constexpr auto bar_h = 1;

                inline constexpr auto description_y = 425;
                inline constexpr auto description_h = 100;
            } // namespace torch

            namespace nightshift
            {
                inline constexpr auto body_offset = style::window::default_vertical_pos + 16;

                inline constexpr auto separator_h = 55;
            } // namespace nightshift

            namespace newApn
            {
                inline constexpr uint32_t x = style::window::default_left_margin;
                inline constexpr uint32_t y = style::window::default_vertical_pos;
                inline constexpr uint32_t w = style::listview::body_width_with_scroll;
                inline constexpr uint32_t h = style::window_height - y - style::footer::height;
            } // namespace newApn

            namespace bluetooth
            {
                namespace passkey
                {
                    namespace image
                    {
                        inline constexpr auto x = 176;
                        inline constexpr auto y = 132;
                    } // namespace image

                    namespace label
                    {
                        inline constexpr auto x = 150U;
                        inline constexpr auto y = 300U;
                        inline constexpr auto w = 200U;
                    } // namespace label

                    namespace text
                    {
                        inline constexpr auto x = 40U;
                        inline constexpr auto y = 370U;
                        inline constexpr auto w = style::window_width - 2 * x;
                        inline constexpr auto h = 52;
                    } // namespace text
                }     // namespace passkey
            }         // namespace bluetooth

            namespace offline
            {
                inline constexpr auto body_offset = 155;

                inline constexpr auto bar_y = 396;
                inline constexpr auto bar_h = 1;

                inline constexpr auto description_y = 375;
                inline constexpr auto description_h = 150;
            } // namespace offline

            namespace sar
            {
                inline constexpr auto LeftMargin = ::style::window::default_left_margin;
                inline constexpr auto TopMargin =
                    ::style::window::default_vertical_pos + ::style::margins::very_big - 1;
                inline constexpr auto Width      = ::style::window::default_body_width;
                inline constexpr auto Height     = ::style::window::default_body_height - ::style::margins::very_big;

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
            inline constexpr auto Height = style::window_height - Y - style::footer::height;
        } // namespace list

        inline constexpr auto cross_x = 48;
        inline constexpr auto cross_y = 55;
        inline constexpr auto arrow_x = 30;
        inline constexpr auto arrow_y = 62;

    } // namespace quotes::categories

    namespace certification
    {
        namespace textfc
        {
            inline constexpr auto x      = 30;
            inline constexpr auto y      = 128;
            inline constexpr auto width  = 128;
            inline constexpr auto height = 35;
        } // namespace textfc
        namespace valuefc
        {
            inline constexpr auto x      = 30;
            inline constexpr auto y      = 166;
            inline constexpr auto width  = 420;
            inline constexpr auto height = 33;
        } // namespace valuefc
        namespace imagefc
        {
            inline constexpr auto x      = 373;
            inline constexpr auto y      = 138;
            inline constexpr auto width  = 64;
            inline constexpr auto height = 44;
        } // namespace imagefc

        namespace textic
        {
            inline constexpr auto x      = 30;
            inline constexpr auto y      = 236;
            inline constexpr auto width  = 133;
            inline constexpr auto height = 35;
        } // namespace textic
        namespace valueic
        {
            inline constexpr auto x      = 30;
            inline constexpr auto y      = 274;
            inline constexpr auto width  = 420;
            inline constexpr auto height = 33;
        } // namespace valueic
        namespace imageic
        {
            inline constexpr auto x      = 373;
            inline constexpr auto y      = 239;
            inline constexpr auto width  = 64;
            inline constexpr auto height = 59;
        } // namespace imageic

        namespace textce
        {
            inline constexpr auto x      = 30;
            inline constexpr auto y      = 339;
            inline constexpr auto width  = 94;
            inline constexpr auto height = 35;
        } // namespace textce
        namespace valuece
        {
            inline constexpr auto x      = 30;
            inline constexpr auto y      = 382;
            inline constexpr auto width  = 420;
            inline constexpr auto height = 33;
        } // namespace valuece
        namespace imagece
        {
            inline constexpr auto x      = 373;
            inline constexpr auto y      = 354;
            inline constexpr auto width  = 64;
            inline constexpr auto height = 46;
        } // namespace imagece

    } // namespace certification

    namespace techinfo
    {
        inline constexpr auto width  = 400;
        inline constexpr auto height = 30;
    } // namespace techinfo
} // namespace style
