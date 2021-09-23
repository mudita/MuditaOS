// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/BedtimeModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>
#include <ctime>
namespace app::bell_bedtime
{
    void BedtimeOnOffModel::setValue(bool value)
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Bedtime::active, valStr, settings::SettingsScope::Global);
    }

    auto BedtimeOnOffModel::getValue() const -> bool
    {
        const auto str = settings.getValue(bell::settings::Bedtime::active, settings::SettingsScope::Global);
        try {
            return std::stoi(str);
        }
        catch (const std::invalid_argument &) {
            return 0;
        }
    }

    auto BedtimeTimeModel::writeString(std::string str) -> void
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

    void BedtimeTimeModel::setValue(time_t value)
    {
        auto tm = std::localtime(&value);
        std::stringstream stringStream;
        stringStream << std::put_time(tm, "%R");
        writeString(stringStream.str());
    }

    auto BedtimeTimeModel::getValue() const -> time_t
    {
        const auto str = readString();
        std::tm tm     = {};
        if (!getTmFromString(str, tm)) {
            // incorrect time string
            getTmFromString(DEFAULT_BEDTIME_TIME, tm);
        }
        return std::mktime(&tm);
    }
} // namespace app::bell_bedtime
