// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <models/alarm_settings/SnoozeSettingsModel.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>

namespace
{
    template <typename T> std::optional<T> get_helper(settings::Settings &settings, const std::string &str)
    {
        const auto retStr = settings.getValue(str, settings::SettingsScope::Global);
        if (retStr.empty()) {
            LOG_ERROR("%s does not exist", str.c_str());
            return {};
        }
        else {
            if constexpr (std::is_integral_v<T>) {
                return utils::toNumeric(retStr);
            }
            else {
                return retStr;
            }
        }
    }
} // namespace

namespace app::bell_settings
{
    void SnoozeOnOffModel::setValue(bool value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::active, valStr, settings::SettingsScope::Global);
    }

    bool SnoozeOnOffModel::getValue() const
    {
        return get_helper<bool>(settings, bell::settings::Snooze::active).value_or(false);
    }

    void SnoozeLengthModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::length, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t SnoozeLengthModel::getValue() const
    {
        return get_helper<std::uint8_t>(settings, bell::settings::Snooze::length).value_or(0);
    }

    void SnoozeChimeIntervalModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::interval, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t SnoozeChimeIntervalModel::getValue() const
    {
        return get_helper<std::uint8_t>(settings, bell::settings::Snooze::interval).value_or(0);
    }

    void SnoozeChimeToneModel::setValue(UTF8 value)
    {
        settings.setValue(bell::settings::Snooze::tone, value, settings::SettingsScope::Global);
    }

    UTF8 SnoozeChimeToneModel::getValue() const
    {
        return get_helper<UTF8>(settings, bell::settings::Snooze::tone).value_or("");
    }

    void SnoozeChimeVolumeModel::setValue(std::uint8_t value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Snooze::volume, valStr, settings::SettingsScope::Global);
    }

    std::uint8_t SnoozeChimeVolumeModel::getValue() const
    {
        return get_helper<std::uint8_t>(settings, bell::settings::Snooze::volume).value_or(0);
    }
} // namespace app::bell_settings
