// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <models/alarm_settings/PrewakeUpSettingsModel.hpp>
#include <db/SystemSettings.hpp>

namespace app::bell_settings
{
    namespace
    {
        inline void validateBrightness(std::string &brightness)
        {
            constexpr auto defaultBrightness = std::string_view{"5"};

            if (brightness.empty()) {
                brightness = defaultBrightness;
            }
        }
    }

    void PrewakeUpChimeDurationModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::PrewakeUp::duration, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t PrewakeUpChimeDurationModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::PrewakeUp::duration, settings::SettingsScope::Global);
        return utils::toNumeric(str);
    }

    void PrewakeUpChimeToneModel::setValue(UTF8 value)
    {
        settings.setValue(bell::settings::PrewakeUp::tonePath, value, settings::SettingsScope::Global);
    }

    UTF8 PrewakeUpChimeToneModel::getValue() const
    {
        return settings.getValue(bell::settings::PrewakeUp::tonePath, settings::SettingsScope::Global);
    }

    void PrewakeUpChimeVolumeModel::setValue(std::uint8_t value)
    {
        audioModel.setVolume(value, AbstractAudioModel::PlaybackType::PreWakeup);
    }

    std::uint8_t PrewakeUpChimeVolumeModel::getValue() const
    {
        return defaultValue;
    }

    PrewakeUpChimeVolumeModel::PrewakeUpChimeVolumeModel(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {
        defaultValue = audioModel.getVolume(AbstractAudioModel::PlaybackType::PreWakeup).value_or(0);
    }

    void PrewakeUpChimeVolumeModel::restoreDefault()
    {
        setValue(defaultValue);
    }

    void PrewakeUpLightDurationModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::PrewakeUp::lightDuration, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t PrewakeUpLightDurationModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::PrewakeUp::lightDuration, settings::SettingsScope::Global);
        return utils::toNumeric(str);
    }

    void PrewakeUpFrontlightModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::PrewakeUp::brightness, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t PrewakeUpFrontlightModel::getValue() const
    {
        auto str = settings.getValue(bell::settings::PrewakeUp::brightness, settings::SettingsScope::Global);
        validateBrightness(str);
        return utils::toNumeric(str);
    }
} // namespace app::bell_settings
