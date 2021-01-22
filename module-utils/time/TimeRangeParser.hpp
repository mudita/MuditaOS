// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Utils.hpp"
#include <module-apps/application-calendar/data/dateCommon.hpp>

namespace utils::time
{
    enum class Version
    {
        Abbrev,
        DayEvent,
        EventDetails
    };

    class TimeRangeParser
    {

      private:
        std::string AMPMtoString(bool isAm);
        std::string timePointToHourMinString(TimePoint time, bool isMode24H, bool addAMPM = false);
        std::string getMultiDayString(TimePoint start, TimePoint end, bool isMode24H);

      public:
        std::string getCalendarTimeString(TimePoint startDate,
                                          TimePoint endDate,
                                          Version version,
                                          bool isMode24H = false);
        static bool isMultiDay(TimePoint start, TimePoint end);
        static uint32_t getDaysCount(TimePoint start, TimePoint end);
        static std::string getDayShortMonthString(TimePoint time);
        static std::pair<TimePoint, TimePoint> getPrepopulatedStartAndEndTime(TimePoint dateFilter);
    };
} // namespace utils::time
