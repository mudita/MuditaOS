// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <models/alarm_settings/SnoozeSettingsModel.hpp>
#include <db/SystemSettings.hpp>

namespace app::bell_settings
{
    auto SnoozeOnOffModel::setValue(bool value) -> void
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::active, valStr, settings::SettingsScope::Global);
    }

    auto SnoozeOnOffModel::getValue() const -> bool
    {
        return utils::getNumericValue<bool>(
            settings.getValue(bell::settings::Snooze::active, settings::SettingsScope::Global));
    }

    auto SnoozeLengthModel::setValue(std::uint8_t value) -> void
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::length, valStr, settings::SettingsScope::Global);
    }

    auto SnoozeLengthModel::getValue() const -> std::uint8_t
    {
        return utils::getNumericValue<std::uint8_t>(
            settings.getValue(bell::settings::Snooze::length, settings::SettingsScope::Global));
    }

    auto SnoozeChimeIntervalModel::setValue(std::uint8_t value) -> void
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::interval, valStr, settings::SettingsScope::Global);
    }

    auto SnoozeChimeIntervalModel::getValue() const -> std::uint8_t
    {
        return utils::getNumericValue<std::uint8_t>(
            settings.getValue(bell::settings::Snooze::interval, settings::SettingsScope::Global));
    }

    SnoozeChimeToneModel::SnoozeChimeToneModel(sys::Service *app, SimpleSoundsRepository &soundsRepository)
        : gui::SettingsModel<UTF8>{app}, soundsRepository{soundsRepository}
    {}

    auto SnoozeChimeToneModel::setValue(UTF8 value) -> void
    {
        const auto &path = soundsRepository.titleToPath(value).value_or("");
        settings.setValue(bell::settings::Snooze::tonePath, path, settings::SettingsScope::Global);
    }

    auto SnoozeChimeToneModel::getValue() const -> UTF8
    {
        const auto &path = settings.getValue(bell::settings::Snooze::tonePath, settings::SettingsScope::Global);
        return soundsRepository.pathToTitle(path).value_or("");
    }

    auto SnoozeChimeVolumeModel::setValue(std::uint8_t value) -> void
    {
        audioModel.setVolume(value, AbstractAudioModel::PlaybackType::Snooze);
    }

    auto SnoozeChimeVolumeModel::getValue() const -> std::uint8_t
    {
        return defaultValue;
    }

    SnoozeChimeVolumeModel::SnoozeChimeVolumeModel(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {
        defaultValue = audioModel.getVolume(AbstractAudioModel::PlaybackType::Snooze).value_or(0);
    }

    auto SnoozeChimeVolumeModel::restoreDefault() -> void
    {
        setValue(defaultValue);
    }
} // namespace app::bell_settings
