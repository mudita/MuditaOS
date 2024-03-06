// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/BedtimeModel.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>
#include <ctime>

namespace
{
    constexpr auto defaultBedtimeTime = "21:00";
}

namespace app::bell_bedtime
{
    auto BedtimeOnOffModel::setValue(bool value) -> void
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Bedtime::active, valStr, settings::SettingsScope::Global);
    }

    auto BedtimeOnOffModel::getValue() const -> bool
    {
        const auto str = settings.getValue(bell::settings::Bedtime::active, settings::SettingsScope::Global);
        return utils::toNumeric(str);
    }

    auto BedtimeTimeModel::writeString(const std::string &str) -> void
    {
        settings.setValue(bell::settings::Bedtime::time, str, settings::SettingsScope::Global);
    }

    auto BedtimeTimeModel::readString() const -> std::string
    {
        return settings.getValue(bell::settings::Bedtime::time, settings::SettingsScope::Global);
    }

    auto BedtimeTimeModel::getTmFromString(const std::string &str, std::tm &tm) const -> bool
    {
        auto stringStream = std::istringstream(str);
        stringStream >> std::get_time(&tm, "%R");
        return bool(stringStream);
    }

    auto BedtimeTimeModel::setValue(std::time_t value) -> void
    {
        auto tm = std::localtime(&value);
        std::stringstream stringStream;
        stringStream << std::put_time(tm, "%R");
        writeString(stringStream.str());
    }

    auto BedtimeTimeModel::getValue() const -> std::time_t
    {
        const auto &str = readString();
        std::tm tm{};
        if (!getTmFromString(str, tm)) {
            // incorrect time string
            getTmFromString(defaultBedtimeTime, tm);
        }
        return std::mktime(&tm);
    }

    BedtimeToneModel::BedtimeToneModel(sys::Service *app, SimpleSoundsRepository &soundsRepository)
        : gui::SettingsModel<UTF8>{app}, soundsRepository{soundsRepository}
    {}

    auto BedtimeToneModel::setValue(UTF8 value) -> void
    {
        const auto &path = soundsRepository.titleToPath(value).value_or("");
        settings.setValue(bell::settings::Bedtime::tonePath, path, settings::SettingsScope::Global);
    }

    auto BedtimeToneModel::getValue() const -> UTF8
    {
        const auto &path = settings.getValue(bell::settings::Bedtime::tonePath, settings::SettingsScope::Global);
        return soundsRepository.pathToTitle(path).value_or("");
    }

    auto BedtimeVolumeModel::setValue(std::uint8_t value) -> void
    {
        audioModel.setVolume(value, AbstractAudioModel::PlaybackType::Bedtime);
    }

    auto BedtimeVolumeModel::getValue() const -> std::uint8_t
    {
        return defaultValue;
    }

    BedtimeVolumeModel::BedtimeVolumeModel(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {
        defaultValue = audioModel.getVolume(AbstractAudioModel::PlaybackType::Bedtime).value_or(0);
    }

    auto BedtimeVolumeModel::restoreDefault() -> void
    {
        setValue(defaultValue);
    }
} // namespace app::bell_bedtime
