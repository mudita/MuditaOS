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

    std::string TimeRangeParser::getCalendarTimeString(TimePoint startDate,
                                                       TimePoint endDate,
                                                       Version version,
                                                       bool isMode24H)
    {
        auto startTime = TimePointToHourMinSec(startDate);
        auto endTime   = TimePointToHourMinSec(endDate);

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

            if (version == Version::Abbrev) {
                if (!isMode24H) {
                    return timePointToHourMinString(startDate, isMode24H) + " " + AMPMtoString(startIsAm);
                }
                return timePointToHourMinString(startDate, isMode24H);
            }
            else if (!isMultiDay(startDate, endDate)) {
                if (!isMode24H) {
                    if (startIsAm != endIsAm) {
                        return timePointToHourMinString(startDate, isMode24H) + " " + AMPMtoString(startIsAm) + " - " +
                               timePointToHourMinString(endDate, isMode24H) + " " + AMPMtoString(endIsAm);
                    }
                    return timePointToHourMinString(startDate, isMode24H) + " - " +
                           timePointToHourMinString(endDate, isMode24H) + " " + AMPMtoString(startIsAm);
                }
                return timePointToHourMinString(startDate, isMode24H) + " - " +
                       timePointToHourMinString(endDate, isMode24H);
            }
            return getMultiDayString(startDate, endDate, isMode24H);
        }
    }

    std::string TimeRangeParser::timePointToHourMinString(TimePoint time, bool isMode24H, bool addAMPM)
    {
        if (isMode24H) {
            return TimePointToHourString24H(time) + ":" + TimePointToMinutesString(time);
        }
        else {
            auto time12h = TimePointToHourString12H(time) + ":" + TimePointToMinutesString(time);
            if (addAMPM) {
                auto hms = TimePointToHourMinSec(time);
                return time12h + " " + AMPMtoString(date::is_am(hms.hours()));
            }
            return time12h;
        }
    }

    std::string TimeRangeParser::getMultiDayString(TimePoint start, TimePoint end, bool isMode24H)
    {
        return TimePointToLocalizedDateString(start, "%a") + " " +
               timePointToHourMinString(start, isMode24H, !isMode24H) + " - " +
               TimePointToLocalizedDateString(end, "%a") + " " + timePointToHourMinString(end, isMode24H, !isMode24H);
    }

    bool TimeRangeParser::isMultiDay(TimePoint start, TimePoint end)
    {
        auto startDate = TimePointToYearMonthDay(start);
        auto endDate   = TimePointToYearMonthDay(end);
        return startDate.day() != endDate.day() || startDate.month() != endDate.month() ||
               startDate.year() != endDate.year();
    }

    uint32_t TimeRangeParser::getDaysCount(TimePoint start, TimePoint end)
    {
        auto eventStartDuration = utils::time::Duration(TimePointToTimeT(start));
        auto eventEndDuration   = utils::time::Duration(TimePointToTimeT(end));
        auto startEventDurationSinceEpochInDaysRoundedDown =
            (eventStartDuration.getHours() - eventStartDuration.getHours() % utils::time::hoursInday) /
            utils::time::hoursInday;
        auto endEventDurationSinceEpochInDaysRoundedDown =
            (eventEndDuration.getHours() - eventEndDuration.getHours() % utils::time::hoursInday) /
            utils::time::hoursInday;
        return endEventDurationSinceEpochInDaysRoundedDown - startEventDurationSinceEpochInDaysRoundedDown;
    }

    std::string TimeRangeParser::getDayShortMonthString(TimePoint time)
    {
        auto ymd = TimePointToYearMonthDay(time);
        return std::to_string(static_cast<unsigned>(ymd.day())) + " " + TimePointToLocalizedDateString(time, "%b");
    }

    std::pair<TimePoint, TimePoint> TimeRangeParser::getPrepopulatedStartAndEndTime(TimePoint dateFilter)
    {
        auto time = TimePointToHourMinSec(TimePointNow());
        if (time.minutes().count() > minutesInHour / 2) {
            dateFilter += std::chrono::hours(time.hours().count() + 1);
        }
        else {
            dateFilter += time.hours() + std::chrono::minutes(minutesInHour / 2);
        }
        return std::make_pair(dateFilter, dateFilter + std::chrono::hours(1));
    }
} // namespace utils::time
