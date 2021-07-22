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
            inline constexpr auto posX   = 0;
            inline constexpr auto posY   = 200;
            inline constexpr auto width  = 480;
            inline constexpr auto height = 100;
            inline constexpr auto font   = "gt_pressura_light_90";
        } // namespace timeLabel

        namespace temperatureLabel
        {
            inline constexpr auto posX   = 0;
            inline constexpr auto posY   = 300;
            inline constexpr auto width  = 480;
            inline constexpr auto height = 50;
            inline constexpr auto font   = "gt_pressura_regular_30";
            inline constexpr auto degree = "\u00B0";
        } // namespace temperatureLabel

        namespace body
        {
            inline constexpr auto posX = 20U;
            inline constexpr auto posY = 45U;
        } // namespace body

    } // namespace mainWindow

    namespace mainMenuWindow
    {
        inline constexpr gui::Length options_list_x     = 40;
        inline constexpr gui::Length options_list_y     = 85;
        inline constexpr gui::Length default_body_width = 400;
    } // namespace mainMenuWindow

    namespace clockSpinner
    {
        inline constexpr auto default_width             = 100;
        inline constexpr auto default_height            = 100;
        inline constexpr auto hour_or_munte_field_width = 2;
    } // namespace clockSpinner

} // namespace bellMainStyle
