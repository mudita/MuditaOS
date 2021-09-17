// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <models/alarm_settings/AlarmSettingsModel.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>

namespace app::bell_settings
{
    void AlarmToneModel::setValue(UTF8 value)
    {
        settings.setValue(bell::settings::Alarm::tone, value, settings::SettingsScope::Global);
    }

    UTF8 AlarmToneModel::getValue() const
    {
        return settings.getValue(bell::settings::Alarm::tone, settings::SettingsScope::Global);
    }

    void AlarmVolumeModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Alarm::volume, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t AlarmVolumeModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::Alarm::volume, settings::SettingsScope::Global);
        return std::stoi(str);
    }

    void AlarmLightOnOffModel::setValue(bool value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Alarm::lightActive, valStr, settings::SettingsScope::Global);
    }

    bool AlarmLightOnOffModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::Alarm::lightActive, settings::SettingsScope::Global);
        return std::stoi(str);
    }
} // namespace app::bell_settings
