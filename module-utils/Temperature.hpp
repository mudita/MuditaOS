// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <iomanip>
#include <optional>
#include <string>

namespace utils::temperature
{
    inline constexpr auto celsiusSymbol          = "C";
    inline constexpr auto fahrenheitSymbol       = "F";
    inline constexpr auto degree                 = "\u00B0";
    inline constexpr auto celsiusDegreeSymbol    = "\u00B0C";
    inline constexpr auto fahrenheitDegreeSymbol = "\u00B0F";

    struct Temperature
    {
        using Value = float;
        enum class Unit
        {
            Celsius,
            Fahrenheit
        };
        Unit unit;
        Value value;
    };

    inline float celsiusToFahrenheit(float value)
    {
        return (value * 1.8) + 32;
    }

    inline std::string unitToStr(Temperature::Unit unit)
    {
        return unit == Temperature::Unit::Celsius ? celsiusSymbol : fahrenheitSymbol;
    }

    inline std::optional<Temperature::Unit> strToUnit(std::string_view str)
    {
        if ((str == celsiusDegreeSymbol) || (str == celsiusSymbol)) {
            return Temperature::Unit::Celsius;
        }
        else if ((str == fahrenheitDegreeSymbol) || (str == fahrenheitSymbol)) {
            return Temperature::Unit::Fahrenheit;
        }
        return {};
    }

    inline std::string tempToStrFloat(Temperature temperature, const int precision = 1)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision);
        stream << temperature.value << degree << utils::temperature::unitToStr(temperature.unit);
        return stream.str();
    }

    inline std::string tempToStrDec(Temperature temperature)
    {
        return std::to_string(static_cast<std::int32_t>(temperature.value)) + degree +
               utils::temperature::unitToStr(temperature.unit);
    }
} // namespace utils::temperature
