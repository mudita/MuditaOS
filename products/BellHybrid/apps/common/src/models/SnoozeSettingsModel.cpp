// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/SnoozeSettingsModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>

namespace app::bell_settings
{
    void SnoozeOnOffModel::setValue(bool value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::active, valStr, settings::SettingsScope::Global);
    }

    bool SnoozeOnOffModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::Snooze::active, settings::SettingsScope::Global);
        return std::stoi(str);
    }

    void SnoozeLengthModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::length, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t SnoozeLengthModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::Snooze::length, settings::SettingsScope::Global);
        return std::stoi(str);
    }

    void SnoozeChimeIntervalModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::interval, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t SnoozeChimeIntervalModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::Snooze::interval, settings::SettingsScope::Global);
        return std::stoi(str);
    }

    void SnoozeChimeToneModel::setValue(UTF8 value)
    {
        settings.setValue(bell::settings::Snooze::tone, value, settings::SettingsScope::Global);
    }

    UTF8 SnoozeChimeToneModel::getValue() const
    {
        return settings.getValue(bell::settings::Snooze::tone, settings::SettingsScope::Global);
    }

    void SnoozeChimeVolumeModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::volume, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t SnoozeChimeVolumeModel::getValue() const
    {
        const auto str = settings.getValue(bell::settings::Snooze::volume, settings::SettingsScope::Global);
        return std::stoi(str);
    }
} // namespace app::bell_settings
