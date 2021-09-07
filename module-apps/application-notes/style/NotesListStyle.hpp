// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/Style.hpp>

namespace app::notes::style::list
{
    constexpr inline auto X      = ::style::window::default_left_margin;
    constexpr inline auto Y      = ::style::window::default_vertical_pos + ::style::margins::small - 1;
    constexpr inline auto Width  = ::style::listview::body_width_with_scroll;
    constexpr inline auto Height = ::style::window::default_body_height - ::style::margins::small;

    constexpr inline auto PenWidth        = 0;
    constexpr inline auto FocusedPenWidth = 0;

    namespace item
    {
        constexpr inline auto Height = 136;
        constexpr inline auto Radius = 0;

        constexpr inline auto VerticalPadding = 16;
        constexpr inline auto LeftPadding     = 10;
        constexpr inline auto RightPadding    = 4;
        namespace title
        {
            constexpr inline auto Width  = 310;
            constexpr inline auto Height = 33;
        } // namespace title

        namespace date
        {
            constexpr inline auto Width  = 90;
            constexpr inline auto Height = 20;
        } // namespace date

        namespace snippet
        {
            constexpr inline auto Width     = 417;
            constexpr inline auto Height    = 66;
            constexpr inline auto TopMargin = 5;
        } // namespace snippet
    }     // namespace item
} // namespace app::notes::style::list
