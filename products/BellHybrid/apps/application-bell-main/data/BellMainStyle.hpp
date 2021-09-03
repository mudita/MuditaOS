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
            inline constexpr auto font   = style::window::font::huge;
            inline constexpr auto width  = 504U;
            inline constexpr auto height = 198U;
        } // namespace time

        namespace alarmSetSpinner
        {
            inline constexpr auto font   = style::window::font::largelight;
            inline constexpr auto width  = 380U;
            inline constexpr auto height = 102U;
        } // namespace alarmSetSpinner

        namespace bottomDescription
        {
            inline constexpr auto font   = style::window::font::largelight;
            inline constexpr auto width  = 380U;
            inline constexpr auto height = 102U;
        } // namespace bottomDescription
    }     // namespace mainWindow

    namespace mainMenuWindow
    {
        inline constexpr gui::Length options_list_x     = 100U;
        inline constexpr gui::Length options_list_y     = 50U;
        inline constexpr gui::Length default_body_width = 400U;
    } // namespace mainMenuWindow
} // namespace bellMainStyle
