// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StaticData.hpp"

namespace stm::internal
{

    StaticData &StaticData::get()
    {
        static StaticData instance;
        return instance;
    }

    void StaticData::setAutomaticDateAndTime(bool value)
    {
        isAutomaticDateAndTimeOn = value;
    }

    bool StaticData::getAutomaticDateAndTime() const noexcept
    {
        return isAutomaticDateAndTimeOn;
    }

    void StaticData::setDateFormat(utils::time::Locale::DateFormat format)
    {
        dateFormat = format;
    }

    utils::time::Locale::DateFormat StaticData::getDateFormat() const noexcept
    {
        return dateFormat;
    }

    void StaticData::setTimeFormat(utils::time::Locale::TimeFormat format)
    {
        timeFormat = format;
    }

    utils::time::Locale::TimeFormat StaticData::getTimeFormat() const noexcept
    {
        return timeFormat;
    }
    void StaticData::setTimezoneName(const std::string &newTimezone)
    {
        timezoneName = newTimezone;
    }
    std::string StaticData::getCurrentTimezoneName() const
    {
        return timezoneName;
    }

    void StaticData::setTimezoneRules(const std::string &newTimezone)
    {
        timezoneRules = newTimezone;
    }
    std::string StaticData::getCurrentTimezoneRules() const
    {
        return timezoneRules;
    }

} // namespace stm::internal
