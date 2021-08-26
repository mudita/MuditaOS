// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/Common.hpp>

namespace bellMainStyle
{
    namespace mainWindow
    {
        namespace time
        {
            inline constexpr auto font = style::window::font::huge;
        } // namespace time

        namespace alarmSetSpinner
        {
            inline constexpr auto font = style::window::font::largelight;
        } // namespace alarmSetSpinner

        namespace bottomDescription
        {
            inline constexpr auto font_normal = style::window::font::largelight;
            inline constexpr auto font_small  = style::window::font::verybiglight;

        } // namespace bottomDescription
    }     // namespace mainWindow

    namespace mainMenuWindow
    {
        inline constexpr gui::Length options_list_x     = 100U;
        inline constexpr gui::Length options_list_y     = 50U;
        inline constexpr gui::Length default_body_width = 400U;
    } // namespace mainMenuWindow
} // namespace bellMainStyle
