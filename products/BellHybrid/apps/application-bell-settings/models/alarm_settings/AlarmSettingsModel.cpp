// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/models/AlarmSettingsModel.hpp>

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
        audioModel.setVolume(value, AbstractAudioModel::PlaybackType::Alarm, {});
    }

    std::uint8_t AlarmVolumeModel::getValue() const
    {
        return defaultValue;
    }
    void AlarmVolumeModel::restoreDefault()
    {
        setValue(defaultValue);
    }
    AlarmVolumeModel::AlarmVolumeModel(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {
        defaultValue = audioModel.getVolume(AbstractAudioModel::PlaybackType::Alarm).value_or(0);
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

    void AlarmFrontlightModel::setValue(frontlight_utils::Brightness value)
    {
        const auto valStr = std::to_string(frontlight_utils::fixedValToPercentage(value));
        settings.setValue(bell::settings::Alarm::brightness, valStr, settings::SettingsScope::Global);
    }

    frontlight_utils::Brightness AlarmFrontlightModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::Alarm::brightness, settings::SettingsScope::Global);
        return frontlight_utils::percentageToFixedVal(std::stoi(str));
    }
} // namespace app::bell_settings
