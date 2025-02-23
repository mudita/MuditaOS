// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/Style.hpp>

namespace app::notes::style::edit
{
    constexpr inline auto LeftMargin = ::style::window::default_left_margin;
    constexpr inline auto TopMargin  = ::style::window::default_vertical_pos + ::style::margins::very_big - 1;
    constexpr inline auto Width      = ::style::window::default_body_width;
    constexpr inline auto Height     = ::style::window::default_body_height - ::style::margins::very_big;

    namespace counter
    {
        constexpr inline auto Height = 30;
    } // namespace counter

    namespace text
    {
        constexpr inline auto TopMargin = 6;
        constexpr inline auto Height    = ::app::notes::style::edit::Height - counter::Height - TopMargin;
    } // namespace text
} // namespace app::notes::style::edit
