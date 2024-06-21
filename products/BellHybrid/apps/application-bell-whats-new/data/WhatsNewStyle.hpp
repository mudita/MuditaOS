// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>
#include "widgets/BellBaseLayout.hpp"

namespace gui::whats_new_style
{
    namespace main_window
    {
        inline constexpr auto list_title_font    = style::window::font::large;
        inline constexpr auto description_font   = style::window::font::mediumbigbold;
        inline constexpr auto description_height = 136U;
    } // namespace main_window

    namespace features_window
    {
        namespace layout
        {
            inline constexpr auto width  = style::window_width;
            inline constexpr auto height = style::window_height;
        } // namespace layout

        namespace container
        {
            inline constexpr auto width      = 544U;
            inline constexpr auto height     = 436U;
            inline constexpr auto margin_top = 42U;
        } // namespace container

        namespace center_box
        {
            inline constexpr auto width  = 448U;
            inline constexpr auto height = 436U;
        } // namespace center_box

        namespace icon
        {
            inline constexpr auto width  = center_box::width;
            inline constexpr auto height = 120U;
        } // namespace icon

        namespace title
        {
            inline constexpr auto font       = style::window::font::large;
            inline constexpr auto width      = center_box::width;
            inline constexpr auto height     = 56U;
            inline constexpr auto margin_top = 16U;
        } // namespace title

        namespace description
        {
            inline constexpr auto font       = style::window::font::verybiglight;
            inline constexpr auto width      = center_box::width;
            inline constexpr auto height     = 168U;
            inline constexpr auto margin_top = 40U;
        } // namespace description
    }     // namespace features_window
} // namespace gui::whats_new_style
