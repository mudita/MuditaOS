// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <i18n/i18n.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    class InputEvent;
}

namespace calc
{
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

            inline const std::string &equals_str()
            {
                return utils::translate("app_calculator_equals");
            }

            inline const std::string &decimal_separator_str()
            {
                return utils::translate("app_calculator_decimal_separator");
            }

            inline const std::string &error_str()
            {
                return utils::translate("app_calculator_error");
            }
        } // namespace strings

    } // namespace symbols

    namespace limits
    {
        inline constexpr auto MaxInputLength   = 7u;
        inline constexpr auto MaxDecimalDigits = 6u;

        inline constexpr auto VeryLowPrecision = 4;
        inline constexpr auto LowPrecision     = 5;
        inline constexpr auto Precision        = 6;
        inline constexpr auto HighPrecision    = 8;
        inline constexpr auto ExpLength        = 1;
        inline constexpr auto MinusExpLength   = 2;
        inline constexpr auto MaxStringLength  = 7;
    } // namespace limits

    class InputProcessor
    {
      public:
        virtual ~InputProcessor() = default;

        virtual bool handle(const gui::InputEvent &event) = 0;
        virtual void clear()                              = 0;

        static bool isSymbol(std::uint32_t code) noexcept;
        static bool isOperation(std::uint32_t code) noexcept;
        static bool isDecimalSeparator(std::uint32_t c) noexcept;
    };
} // namespace calc
