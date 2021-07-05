// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
            return utils::translate(utils::time::Locale::getAM());
        }
        return utils::translate(utils::time::Locale::getPM());
    }

    std::string TimeRangeParser::getCalendarTimeString(TimePoint startDate,
                                                       TimePoint endDate,
                                                       Version version,
                                                       bool isMode24H)
    {
        auto startTime = TimePointToHourMinSec(startDate);
        auto endTime   = TimePointToHourMinSec(endDate);

        if (startTime.hours().count() == 0 && startTime.minutes().count() == 0 && endTime.hours().count() == max_hour &&
            endTime.minutes().count() == max_minutes) {
            return utils::translate("app_calendar_all_day");
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
                    return TimePointToHourString12H(startDate) + ":" + TimePointToMinutesString(startDate) + " " +
                           AMPMtoString(startIsAm);
                }
                else {
                    return TimePointToHourString24H(startDate) + ":" + TimePointToMinutesString(startDate);
                }
            }
            else {
                if (!isMode24H) {
                    if (startIsAm != endIsAm) {
                        return TimePointToHourString12H(startDate) + ":" + TimePointToMinutesString(startDate) + " " +
                               AMPMtoString(startIsAm) + " - " + TimePointToHourString12H(endDate) + ":" +
                               TimePointToMinutesString(endDate) + " " + AMPMtoString(endIsAm);
                    }
                    else {
                        return TimePointToHourString12H(startDate) + ":" + TimePointToMinutesString(startDate) + " - " +
                               TimePointToHourString12H(endDate) + ":" + TimePointToMinutesString(endDate) + " " +
                               AMPMtoString(startIsAm);
                    }
                }
                else {
                    return TimePointToHourString24H(startDate) + ":" + TimePointToMinutesString(startDate) + " - " +
                           TimePointToHourString24H(endDate) + ":" + TimePointToMinutesString(endDate);
                }
            }
        }
    }
} // namespace utils::time
