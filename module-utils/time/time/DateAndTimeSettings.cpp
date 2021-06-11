// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateAndTimeSettings.hpp"

namespace utils
{
    DateAndTimeSettings dateAndTimeSettings;

    void DateAndTimeSettings::setAutomaticDateAndTimeOn(bool value)
    {
        automaticDateAndTimeIsOn = value;
    }

    void DateAndTimeSettings::setAutomaticTimeZoneOn(bool value)
    {
        automaticTimeZoneIsOn = value;
    }

    void DateAndTimeSettings::setTimeFormat(time::Locale::TimeFormat format)
    {
        timeFormat = format;
    }

    void DateAndTimeSettings::setDateFormat(time::Locale::DateFormat format)
    {
        dateFormat = format;
    }
} // namespace utils
