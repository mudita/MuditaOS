// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <models/alarm_settings/PrewakeUpSettingsModel.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>

namespace app::bell_settings
{
    void PrewakeUpChimeDurationModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::PrewakeUp::duration, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t PrewakeUpChimeDurationModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::PrewakeUp::duration, settings::SettingsScope::Global);
        return std::stoi(str);
    }

    void PrewakeUpChimeToneModel::setValue(UTF8 value)
    {
        settings.setValue(bell::settings::PrewakeUp::tone, value, settings::SettingsScope::Global);
    }

    UTF8 PrewakeUpChimeToneModel::getValue() const
    {
        return settings.getValue(bell::settings::PrewakeUp::tone, settings::SettingsScope::Global);
    }

    void PrewakeUpChimeVolumeModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::PrewakeUp::volume, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t PrewakeUpChimeVolumeModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::PrewakeUp::volume, settings::SettingsScope::Global);
        return std::stoi(str);
    }

    void PrewakeUpLightDurationModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::PrewakeUp::lightDuration, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t PrewakeUpLightDurationModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::PrewakeUp::lightDuration, settings::SettingsScope::Global);
        return std::stoi(str);
    }
} // namespace app::bell_settings
