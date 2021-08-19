// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TemperatureModel.hpp"

namespace
{
    inline constexpr auto celsiusDegreeSymbol    = "C";
    inline constexpr auto fahrenheitDegreeSymbol = "F";
} // namespace

namespace app::home_screen
{

    home_screen::TemperatureModel::TemperatureModel(app::Application *)
    {}
    AbstractTemperatureModel::Temperature TemperatureModel::getTemperature() const
    {
        // This is just a placeholder for missing functionality
        return {AbstractTemperatureModel::Temperature::Unit::Celsius, 25};
    }

    std::string AbstractTemperatureModel::unitToStr(Temperature::Unit unit)
    {
        return unit == AbstractTemperatureModel::Temperature::Unit::Celsius ? celsiusDegreeSymbol
                                                                            : fahrenheitDegreeSymbol;
    }
} // namespace app::home_screen
