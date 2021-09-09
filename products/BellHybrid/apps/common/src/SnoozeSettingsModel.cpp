// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/SnoozeSettingsModel.hpp"

#include <apps-common/Application.hpp>
#include <db/SystemSettings.hpp>

namespace app::bell_settings
{
    SnoozeSettingsModel::SnoozeSettingsModel(Application *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }
    bool SnoozeSettingsModel::getSnooze() const
    {
        const auto str = settings.getValue(bell::settings::Snooze::active, settings::SettingsScope::Global);
        return std::stoi(str);
    }
    void SnoozeSettingsModel::setSnooze(bool value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::active, valStr, settings::SettingsScope::Global);
    }
    void SnoozeSettingsModel::setLength(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::length, valStr, settings::SettingsScope::Global);
    }
    std::uint8_t SnoozeSettingsModel::getLength() const
    {
        const auto str = settings.getValue(bell::settings::Snooze::length, settings::SettingsScope::Global);
        return std::stoi(str);
    }
    void SnoozeSettingsModel::setChimeInterval(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::interval, valStr, settings::SettingsScope::Global);
    }
    std::uint8_t SnoozeSettingsModel::getChimeInterval() const
    {
        const auto str = settings.getValue(bell::settings::Snooze::interval, settings::SettingsScope::Global);
        return std::stoi(str);
    }
    void SnoozeSettingsModel::setChimeTone(const UTF8 &value)
    {
        settings.setValue(bell::settings::Snooze::tone, value, settings::SettingsScope::Global);
    }
    UTF8 SnoozeSettingsModel::getChimeTone() const
    {
        return settings.getValue(bell::settings::Snooze::tone, settings::SettingsScope::Global);
    }
    void SnoozeSettingsModel::setChimeVolume(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::volume, valStr, settings::SettingsScope::Global);
    }
    std::uint8_t SnoozeSettingsModel::getChimeVolume() const
    {
        const auto str = settings.getValue(bell::settings::Snooze::volume, settings::SettingsScope::Global);
        return std::stoi(str);
    }
} // namespace app::bell_settings
