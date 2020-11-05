// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeRangeParser.hpp"

namespace utils::time
{
    namespace
    {
        constexpr int max_hour    = 23;
        constexpr int max_minutes = 59;
    } // namespace

    std::string TimeRangeParser::AMPMtoString(bool isAm)
    {
        if (isAm) {
            return utils::localize.get(utils::time::Locale::getAM());
        }
        return utils::localize.get(utils::time::Locale::getPM());
    }

    std::string TimeRangeParser::getCalendarTimeString(utils::time::TimePoint startDate,
                                                       utils::time::TimePoint endDate,
                                                       Version version,
                                                       bool isMode24H)
    {
        auto startTime = CalendarConversion::TimePointToHourMinSec(startDate);
        auto endTime   = CalendarConversion::TimePointToHourMinSec(endDate);

        if (startTime.hours().count() == 0 && startTime.minutes().count() == 0 && endTime.hours().count() == max_hour &&
            endTime.minutes().count() == max_minutes) {
            return utils::localize.get("app_calendar_all_day");
        }
        else {
            bool startIsAm = true;
            bool endIsAm   = true;

            if (!isMode24H) {
                startIsAm = date::is_am(startTime.hours());
                endIsAm   = date::is_am(endTime.hours());
            }

            if (version == Version::abbrev) {
                if (!isMode24H) {
                    return CalendarConversion::TimePointToHourString12H(startDate) + ":" +
                           CalendarConversion::TimePointToMinutesString(startDate) + " " + AMPMtoString(startIsAm);
                }
                else {
                    return CalendarConversion::TimePointToHourString24H(startDate) + ":" +
                           CalendarConversion::TimePointToMinutesString(startDate);
                }
            }
            else {
                if (!isMode24H) {
                    if (startIsAm != endIsAm) {
                        return CalendarConversion::TimePointToHourString12H(startDate) + ":" +
                               CalendarConversion::TimePointToMinutesString(startDate) + " " + AMPMtoString(startIsAm) +
                               " - " + CalendarConversion::TimePointToHourString12H(endDate) + ":" +
                               CalendarConversion::TimePointToMinutesString(endDate) + " " + AMPMtoString(endIsAm);
                    }
                    else {
                        return CalendarConversion::TimePointToHourString12H(startDate) + ":" +
                               CalendarConversion::TimePointToMinutesString(startDate) + " - " +
                               CalendarConversion::TimePointToHourString12H(endDate) + ":" +
                               CalendarConversion::TimePointToMinutesString(endDate) + " " + AMPMtoString(startIsAm);
                    }
                }
                else {
                    return CalendarConversion::TimePointToHourString24H(startDate) + ":" +
                           CalendarConversion::TimePointToMinutesString(startDate) + " - " +
                           CalendarConversion::TimePointToHourString24H(endDate) + ":" +
                           CalendarConversion::TimePointToMinutesString(endDate);
                }
            }
        }
    }
} // namespace utils::time
