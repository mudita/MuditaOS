// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TemperatureUnitModel.hpp"

#include <apps-common/Application.hpp>
#include <service-db/Settings.hpp>
#include <db/SystemSettings.hpp>

namespace
{
    constexpr auto temperatureUnit = bell::settings::Temperature::unit;
} // namespace

namespace app::bell_settings
{
    TemperatureUnitModel::TemperatureUnitModel(app::Application *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    utils::temperature::Temperature::Unit TemperatureUnitModel::get() const
    {
        const auto value = settings.getValue(temperatureUnit, settings::SettingsScope::Global);
        if (auto unit = utils::temperature::strToUnit(value); unit) {
            return *unit;
        }
        else {
            return utils::temperature::Temperature::Unit::Celsius;
        }
    }

    void TemperatureUnitModel::set(utils::temperature::Temperature::Unit unit)
    {
        settings.setValue(temperatureUnit, utils::temperature::unitToStr(unit), settings::SettingsScope::Global);
    }

} // namespace app::bell_settings
