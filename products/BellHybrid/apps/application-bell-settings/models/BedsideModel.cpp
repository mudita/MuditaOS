// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <common/models/BedsideModel.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <Utils.hpp>

namespace app::bell_settings
{
    auto BedsideBrightnessModel::setValue(std::uint8_t value) -> void
    {
        const auto str = std::to_string(value);
        settings.setValue(settings::Brightness::bedsideBrightnessLevel, str, settings::SettingsScope::Global);
    }

    auto BedsideBrightnessModel::getValue() const -> std::uint8_t
    {
        const auto str =
            settings.getValue(settings::Brightness::bedsideBrightnessLevel, settings::SettingsScope::Global);
        return utils::toNumeric(str);
    }

    auto BedsideTimeModel::setValue(std::uint8_t value) -> void
    {
        const auto str = std::to_string(value);
        settings.setValue(settings::Brightness::bedsideTime, str, settings::SettingsScope::Global);
    }

    auto BedsideTimeModel::getValue() const -> std::uint8_t
    {
        const auto str = settings.getValue(settings::Brightness::bedsideTime, settings::SettingsScope::Global);
        return utils::toNumeric(str);
    }
} // namespace app::bell_settings
