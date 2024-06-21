// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/Common.hpp>

namespace bellMainStyle
{
    namespace mainWindow
    {
        namespace time
        {
            inline constexpr auto font = style::window::font::gargantuan;
            inline constexpr auto font_small = style::window::font::veryverybiglight;
        } // namespace time

        namespace alarmSetSpinner
        {
            inline constexpr auto font = style::window::font::largelight;
        } // namespace alarmSetSpinner

        namespace bottomDescription
        {
            inline constexpr auto font_normal = style::window::font::largelight;
            inline constexpr auto font_smallnormal = style::window::font::veryverybiglight;
            inline constexpr auto font_small  = style::window::font::verybiglight;

        } // namespace bottomDescription

        namespace ampm
        {
            inline constexpr auto layout_w      = 51U;
            inline constexpr auto margin_w      = 5U;
            inline constexpr auto time_margin_w = layout_w + margin_w;
        } // namespace ampm
    } // namespace mainWindow
} // namespace bellMainStyle
