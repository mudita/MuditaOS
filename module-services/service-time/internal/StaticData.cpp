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

    void StaticData::setAutomaticTimezoneOn(bool value)
    {
        isAutomaticTimezoneOn = value;
    }

    bool StaticData::getAutomaticTimezone() const noexcept
    {
        return isAutomaticTimezoneOn;
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
    void StaticData::setTimezone(const std::string &newTimezone)
    {
        timezone = newTimezone;
    }
    std::string StaticData::getCurrentTimezone() const
    {
        return timezone;
    }

} // namespace stm::internal