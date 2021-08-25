// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/Common.hpp>

namespace bellMainStyle
{
    namespace mainWindow
    {
        namespace timeLabel
        {
            inline constexpr auto font = "gt_pressura_light_90";
        } // namespace timeLabel

        namespace alarmSetSpinner
        {
            inline constexpr auto font = style::window::font::largelight;
        } // namespace alarmSetSpinner

        namespace temperatureLabel
        {
            inline constexpr auto font   = style::window::font::largelight;
        } // namespace temperatureLabel
    }     // namespace mainWindow

    namespace mainMenuWindow
    {
        inline constexpr gui::Length options_list_x     = 40;
        inline constexpr gui::Length options_list_y     = 85;
        inline constexpr gui::Length default_body_width = 400;
    } // namespace mainMenuWindow
} // namespace bellMainStyle
