// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Style.hpp>
#include <string>

namespace style
{
    namespace settings
    {
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
        }     // namespace window
    };        // namespace settings
} // namespace style
