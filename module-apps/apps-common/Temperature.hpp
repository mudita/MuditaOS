// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace gui::temperature
{
    inline constexpr auto celsiusSymbol          = "C";
    inline constexpr auto fahrenheitSymbol       = "F";
    inline constexpr auto degree                 = "\u00B0";
    inline constexpr auto celsiusDegreeSymbol    = "\u00B0C";
    inline constexpr auto fahrenheitDegreeSymbol = "\u00B0F";

    struct Temperature
    {
        enum class Unit
        {
            Celsius,
            Fahrenheit
        };
        Unit unit;
        std::int32_t value;
    };

    inline std::string toStr(Temperature::Unit unit)
    {
        return unit == Temperature::Unit::Celsius ? celsiusSymbol : fahrenheitSymbol;
    }

    inline std::optional<Temperature::Unit> strToUnit(std::string_view str)
    {
        if (str == gui::temperature::celsiusDegreeSymbol) {
            return gui::temperature::Temperature::Unit::Celsius;
        }
        else if (str == gui::temperature::fahrenheitDegreeSymbol) {
            return gui::temperature::Temperature::Unit::Fahrenheit;
        }

        return {};
    }
} // namespace gui::temperature
