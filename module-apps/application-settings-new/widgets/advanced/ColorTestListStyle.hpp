// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace style::colorTest
{
    namespace listview
    {
        inline constexpr auto x      = style::window::default_left_margin;
        inline constexpr auto y      = style::window::default_vertical_pos;
        inline constexpr auto width  = style::listview::body_width_with_scroll;
        inline constexpr auto height = style::window_height - y - style::footer::height;
    } // namespace listview

    namespace item
    {
        namespace label
        {
            inline constexpr auto height       = 30;
            inline constexpr auto width        = style::listview::item_width_with_scroll;
            inline constexpr auto bottomMargin = 4;
            inline constexpr auto leftMargin   = 10;
        } // namespace label

        namespace color
        {
            inline constexpr auto height       = listview::height - label::height;
            inline constexpr auto width        = style::listview::item_width_with_scroll;
            inline constexpr auto bottomMargin = 4;
            inline constexpr auto leftMargin   = 10;
        } // namespace color
    }     // namespace item
} // namespace style::colorTest
