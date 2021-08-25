// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TemperatureUnitModel.hpp"

#include <apps-common/Application.hpp>
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace
{
    constexpr auto temperatureUnit = settings::Temperature::unit;
} // namespace

namespace app::bell_settings
{
    TemperatureUnitModel::TemperatureUnitModel(app::Application *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    gui::temperature::Temperature::Unit TemperatureUnitModel::get() const
    {
        const auto value = settings.getValue(temperatureUnit, settings::SettingsScope::Global);
        return value == gui::temperature::celsiusSymbol ? gui::temperature::Temperature::Unit::Celsius
                                                        : gui::temperature::Temperature::Unit::Fahrenheit;
    }

    void TemperatureUnitModel::set(gui::temperature::Temperature::Unit unit)
    {
        settings.setValue(gui::temperature::toStr(unit), temperatureUnit, settings::SettingsScope::Global);
    }

} // namespace app::bell_settings
