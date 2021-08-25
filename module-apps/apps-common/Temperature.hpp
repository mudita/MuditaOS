// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <string>

namespace gui::temperature
{
    inline constexpr auto celsiusDegreeSymbol    = "C";
    inline constexpr auto fahrenheitDegreeSymbol = "F";
    inline constexpr auto degree                 = "\u00B0";

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
        return unit == Temperature::Unit::Celsius ? celsiusDegreeSymbol : fahrenheitDegreeSymbol;
    }
} // namespace gui::temperature
