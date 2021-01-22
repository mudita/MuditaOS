// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
            namespace leftArrowImage
            {
                inline constexpr auto x = style::window::default_left_margin;
                inline constexpr auto y = 62;
                inline constexpr auto w = 11;
                inline constexpr auto h = 13;
            } // namespace leftArrowImage

            namespace crossImage
            {
                inline constexpr auto x = style::window::default_left_margin + 20;
                inline constexpr auto y = 55;
                inline constexpr auto w = 24;
                inline constexpr auto h = 24;
            } // namespace crossImage
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
                inline constexpr auto body_offset = style::header::height + 16;

                inline constexpr auto separator_h = 55;
            } // namespace nightshift

            namespace newApn
            {
                inline constexpr uint32_t x = style::window::default_left_margin;
                inline constexpr uint32_t y = style::header::height;
                inline constexpr uint32_t w = style::listview::body_width_with_scroll;
                inline constexpr uint32_t h = style::window_height - y - style::footer::height;
            } // namespace newApn

        }     // namespace window
    };        // namespace settings
} // namespace style
