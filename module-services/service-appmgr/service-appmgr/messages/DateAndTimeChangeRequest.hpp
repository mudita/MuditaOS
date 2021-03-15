// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"
#include <time/time_locale.hpp>

namespace app::manager
{
    class DateAndTimeChangeRequest : public sys::DataMessage
    {};

    class AutomaticDateAndTimeIsOnChangeRequest : public DateAndTimeChangeRequest
    {
      public:
        AutomaticDateAndTimeIsOnChangeRequest(bool isOn) : isOn(isOn)
        {}
        bool isOn;
    };

    class AutomaticTimeZoneIsOnChangeRequest : public DateAndTimeChangeRequest
    {
      public:
        AutomaticTimeZoneIsOnChangeRequest(bool isOn) : isOn(isOn)
        {}
        bool isOn;
    };

    class TimeFormatChangeRequest : public DateAndTimeChangeRequest
    {
      public:
        TimeFormatChangeRequest(utils::time::Locale::TimeFormat timeFormat) : timeFormat(timeFormat)
        {}
        utils::time::Locale::TimeFormat timeFormat;
    };

    class DateFormatChangeRequest : public DateAndTimeChangeRequest
    {
      public:
        DateFormatChangeRequest(utils::time::Locale::DateFormat dateFormat) : dateFormat(dateFormat)
        {}
        utils::time::Locale::DateFormat dateFormat;
    };
} // namespace app::manager
