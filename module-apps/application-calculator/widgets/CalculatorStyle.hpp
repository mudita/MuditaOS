// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/Style.hpp>

namespace style::calculator
{
    inline constexpr auto grid_cells        = 9;
    inline constexpr auto equals            = "app_calculator_equals";
    inline constexpr auto decimal_separator = "app_calculator_decimal_separator";

    namespace window
    {
        inline constexpr auto math_box_height      = 240;
        inline constexpr auto math_box_offset_top  = style::header::height + 200;
        inline constexpr auto math_box_cell_height = 80;
        inline constexpr auto math_box_cell_width  = style::window::default_body_width / 3;
        inline constexpr auto input_offset_top     = style::header::height + 20;
        inline constexpr auto input_height         = 100;
        inline constexpr auto input_width          = 380;
        inline constexpr auto input_margin         = 50;
    } // namespace window

    namespace symbols
    {
        namespace codes
        {
            inline constexpr auto plus           = 0x002B;
            inline constexpr auto minus          = 0x002D;
            inline constexpr auto division       = 0x00F7;
            inline constexpr auto multiplication = 0x00D7;
            inline constexpr auto full_stop      = 0x002E;
            inline constexpr auto comma          = 0x002C;
            inline constexpr auto equals         = 0x003D;
            inline constexpr auto zero           = 0x0030;
        } // namespace codes

        namespace strings
        {
            inline constexpr auto plus           = "\u002B";
            inline constexpr auto minus          = "\u002D";
            inline constexpr auto division       = "\u00F7";
            inline constexpr auto multiplication = "\u00D7";
            inline constexpr auto equals         = "\u003D";
            inline constexpr auto full_stop      = "\u002E";
            inline constexpr auto comma          = "\u002C";
            inline constexpr auto asterisk       = "\u002A";
            inline constexpr auto solidus        = "\u002F";
        } // namespace strings
    }     // namespace symbols
} // namespace style::calculator
