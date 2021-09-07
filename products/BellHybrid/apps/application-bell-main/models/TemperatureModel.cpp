// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TemperatureModel.hpp"

#include <apps-common/Application.hpp>
#include <service-db/Settings.hpp>
#include <db/SystemSettings.hpp>
#include <evtmgr/api/TemperatureApi.hpp>

namespace app::home_screen
{
    TemperatureModel::TemperatureModel(app::Application *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }
    utils::temperature::Temperature TemperatureModel::getTemperature() const
    {
        const auto unitStr = settings.getValue(bell::settings::Temperature::unit, settings::SettingsScope::Global);
        const auto unit    = *utils::temperature::strToUnit(unitStr);
        auto temperature   = evtmgr::api::getCurrentTemperature();

        if (unit == utils::temperature::Temperature::Unit::Fahrenheit) {
            temperature = utils::temperature::celsiusToFahrenheit(temperature);
        }
        return {unit, temperature};
    }
} // namespace app::home_screen
