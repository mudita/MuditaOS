#pragma once
#include <string>
#include <module-gui/gui/widgets/Style.hpp>

namespace style::calculator
{
    const inline int grid_cells                = 9;
    const inline std::string equals            = "app_calculator_equals";
    const inline std::string decimal_separator = "app_calculator_decimal_separator";

    namespace window
    {
        const inline int math_box_height      = 240;
        const inline int math_box_offset_top  = style::header::height + 200;
        const inline int math_box_cell_height = 80;
        const inline int math_box_cell_width  = style::window::default_body_width / 3;
        const inline int input_offset_top     = style::header::height + 20;
        const inline int input_height         = 100;
        const inline int input_width          = 380;
        const inline int input_margin         = 50;
    } // namespace window

    namespace symbols
    {
        namespace codes
        {
            const inline int plus           = 0x002B;
            const inline int minus          = 0x002D;
            const inline int division       = 0x00F7;
            const inline int multiplication = 0x00D7;
            const inline int full_stop      = 0x002E;
            const inline int comma          = 0x002C;
            const inline int equals         = 0x003D;
        } // namespace codes

        namespace strings
        {
            const inline std::string plus           = "\u002B";
            const inline std::string minus          = "\u002D";
            const inline std::string division       = "\u00F7";
            const inline std::string multiplication = "\u00D7";
            const inline std::string equals         = "\u003D";
            const inline std::string full_stop      = "\u002E";
            const inline std::string comma          = "\u002C";
            const inline std::string asterisk       = "\u002A";
            const inline std::string solidus        = "\u002F";
        } // namespace strings
    }     // namespace symbols
} // namespace style::calculator
