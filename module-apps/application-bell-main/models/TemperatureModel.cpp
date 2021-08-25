// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TemperatureModel.hpp"

namespace app::home_screen
{

    home_screen::TemperatureModel::TemperatureModel(app::Application *)
    {}
    gui::temperature::Temperature TemperatureModel::getTemperature() const
    {
        // This is just a placeholder for missing functionality
        return {gui::temperature::Temperature::Unit::Celsius, 25};
    }
} // namespace app::home_screen
