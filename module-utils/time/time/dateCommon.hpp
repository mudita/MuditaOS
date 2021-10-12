// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef DATECOMMON_H
#define DATECOMMON_H

#include "time_conversion_factory.hpp"

#include <Utils.hpp>

#include <date/date.h>

#include <random>

using Clock     = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace calendar
{
    using YearMonthDay     = date::year_month_day;
    using YearMonthDayLast = date::year_month_day_last;

    class Timestamp : public utils::time::Timestamp
    {
      public:
        enum class GetParameters
        {
            Hour,
            Minute,
            Day,
            Month,
            Year
        };

        explicit Timestamp(time_t newtime) : utils::time::Timestamp(newtime)
        {}

        uint32_t get_date_time_sub_value(GetParameters param)
        {
            auto timeinfo = *std::localtime(&time);
            switch (param) {
            case GetParameters::Hour:
                return timeinfo.tm_hour;
            case GetParameters::Minute:
                return timeinfo.tm_min;
            case GetParameters::Day:
                return timeinfo.tm_mday;
            case GetParameters::Month:
                return timeinfo.tm_mon + 1;
            case GetParameters::Year:
                return timeinfo.tm_year + 1900;
            }
            return UINT32_MAX;
        }
        uint32_t get_UTC_date_time_sub_value(GetParameters param)
        {
            std::tm tm = *std::gmtime(&time);
            switch (param) {
            case GetParameters::Hour:
                return tm.tm_hour;
            case GetParameters::Minute:
                return tm.tm_min;
            case GetParameters::Day:
                return tm.tm_mday;
            case GetParameters::Month:
                return tm.tm_mon + 1;
            case GetParameters::Year:
                return tm.tm_year + 1900;
            }
            return UINT32_MAX;
        }
    };
} // namespace calendar

inline constexpr auto max_month_day   = 31;
inline constexpr auto unix_epoch_year = 1900;

enum class Reminder
{
    never              = 0xFFFF,
    event_time         = 0,
    five_min_before    = 5,
    fifteen_min_before = 15,
    thirty_min_before  = 30,
    one_hour_before    = 60,
    two_hour_before    = 120,
    one_day_before     = 1440,
    two_days_before    = 2880,
    one_week_before    = 10080
};

enum class Repeat
{
    never,
    daily,
    weekly,
    biweekly,
    monthly,
    yearly
};

inline constexpr TimePoint TIME_POINT_INVALID = date::sys_days{date::January / 1 / 1970};
inline constexpr TimePoint TIME_POINT_MAX     = date::sys_days{date::April / 11 / 2262};
inline constexpr uint32_t yearDigitsNumb = 4, monthDigitsNumb = 2, dayDigitsNumb = 2, HourDigitsNumb = 2,
                          MinDigitsNumb = 2, SecDigitsNumb = 2;

inline std::tm CreateTmStruct(int year, int month, int day, int hour, int minutes, int seconds)
{
    struct tm tm_ret;

    tm_ret.tm_isdst = -1;
    tm_ret.tm_sec   = seconds;
    tm_ret.tm_min   = minutes;
    tm_ret.tm_hour  = hour;
    tm_ret.tm_mday  = day;
    tm_ret.tm_mon   = month - 1;
    tm_ret.tm_year  = year - 1900;

    return tm_ret;
}

inline time_t GetDiffLocalWithUTCTime()
{
    std::tm tm = CreateTmStruct(2000, 1, 1, 0, 0, 0);

    std::time_t basetime = std::mktime(&tm);
    std::time_t diff;

    tm          = *std::localtime(&basetime);
    tm.tm_isdst = -1;
    diff        = std::mktime(&tm);

    tm          = *std::gmtime(&basetime);
    tm.tm_isdst = -1;
    diff -= std::mktime(&tm);

    return diff;
}

inline time_t GetAsUTCTime(int year, int month, int day, int hour = 0, int minutes = 0, int seconds = 0)
{
    std::tm tm           = CreateTmStruct(year, month, day, hour, minutes, seconds);
    std::time_t basetime = std::mktime(&tm);

    return basetime + GetDiffLocalWithUTCTime();
}

inline TimePoint TimePointFromTimeT(const time_t &time)
{
    return std::chrono::system_clock::from_time_t(time);
}

inline time_t TimePointToTimeT(const TimePoint &tp)
{
    return std::chrono::system_clock::to_time_t(tp);
}

inline TimePoint TimePointNow()
{
    return TimePointFromTimeT(std::time(nullptr));
}

inline std::string TimePointToString(const TimePoint &tp)
{
    return date::format("%F %T", std::chrono::time_point_cast<std::chrono::seconds>(tp));
}

inline auto TimePointToHourMinSec(const TimePoint &tp)
{
    auto dp = date::floor<date::days>(tp);
    return date::make_time(tp - dp);
}

inline uint32_t TimePointToHour24H(const TimePoint &tp)
{
    auto time = TimePointToTimeT(tp);
    calendar::Timestamp timestamp(time);
    auto hour = timestamp.get_date_time_sub_value(calendar::Timestamp::GetParameters::Hour);
    return hour;
}

inline auto LocalizedHoursToUtcHours(int hour = 0)
{
    std::tm tm           = CreateTmStruct(unix_epoch_year, 1, 1, hour, 0, 0);
    std::time_t basetime = std::mktime(&tm);
    basetime -= GetDiffLocalWithUTCTime();
    return TimePointToHour24H(TimePointFromTimeT(basetime));
}

inline uint32_t TimePointToMinutes(const TimePoint &tp)
{
    auto time = TimePointToTimeT(tp);
    calendar::Timestamp timestamp(time);
    auto minute = timestamp.get_date_time_sub_value(calendar::Timestamp::GetParameters::Minute);
    return minute;
}

inline TimePoint getFirstWeekDay(const TimePoint &tp)
{
    auto time_of_day  = TimePointToHourMinSec(tp);
    auto yearMonthDay = date::year_month_day{date::floor<date::days>(tp)};
    while (date::weekday{yearMonthDay} != date::mon) {
        auto decrementedDay = --yearMonthDay.day();
        yearMonthDay        = yearMonthDay.year() / yearMonthDay.month() / decrementedDay;
    }
    auto finalDate     = date::sys_days{yearMonthDay.year() / yearMonthDay.month() / yearMonthDay.day()};
    auto finalDateTime = finalDate + time_of_day.hours() + time_of_day.minutes();

    return finalDateTime;
}

inline std::string TimePointToString(const TimePoint &tp, date::months months)
{
    date::year_month_day yearMonthDay     = date::year_month_day{date::floor<date::days>(tp)};
    date::year_month_day yearMonthDayLast = yearMonthDay.year() / yearMonthDay.month() / date::last;
    auto tpHourMinuteSecond               = TimePointToHourMinSec(tp).to_duration();

    TimePoint timePoint;

    if ((static_cast<unsigned>(yearMonthDay.month()) + months.count()) <= 12) {
        if (yearMonthDayLast.day() == yearMonthDay.day()) {
            yearMonthDayLast = yearMonthDay.year() / (yearMonthDay.month() + months) / date::last;
            timePoint = date::sys_days{yearMonthDayLast.year() / yearMonthDayLast.month() / yearMonthDayLast.day()};
        }
        else {
            timePoint = date::sys_days{yearMonthDay.year() / (yearMonthDay.month() + months) / yearMonthDay.day()};
        }
    }
    else {
        date::month incrementedMonths = date::month(months.count()) - (date::month(12) - yearMonthDay.month());
        yearMonthDay                  = (yearMonthDay.year() + date::years{1}) / incrementedMonths / yearMonthDay.day();
        if (yearMonthDayLast.day() == yearMonthDay.day()) {
            yearMonthDayLast = yearMonthDay.year() / incrementedMonths / date::last;
            timePoint = date::sys_days{yearMonthDayLast.year() / yearMonthDayLast.month() / yearMonthDayLast.day()};
        }
        else {
            timePoint = date::sys_days{yearMonthDay.year() / yearMonthDay.month() / yearMonthDay.day()};
        }
    }

    auto time_of_day = TimePointToHourMinSec(tp);
    return date::format(
        "%F %T",
        std::chrono::time_point_cast<std::chrono::seconds>(timePoint + time_of_day.hours() + time_of_day.minutes()));
}

inline std::string TimePointToString(const TimePoint &tp, date::years years)
{
    auto yearMonthDay     = date::year_month_day{date::floor<date::days>(tp)};
    auto yearMonthDayLast = (yearMonthDay.year() + date::years(years)) / yearMonthDay.month() / date::last;

    TimePoint timePoint =
        date::sys_days{yearMonthDayLast.year() / yearMonthDayLast.month() /
                       ((yearMonthDayLast.day() == yearMonthDay.day()) ? yearMonthDayLast.day() : yearMonthDay.day())};

    auto time_of_day = TimePointToHourMinSec(tp);
    return date::format(
        "%F %T",
        std::chrono::time_point_cast<std::chrono::seconds>(timePoint + time_of_day.hours() + time_of_day.minutes()));
}

inline std::string HHMMToLocalizedString(std::chrono::hours hours,
                                         std::chrono::minutes minutes,
                                         utils::time::TimestampType type,
                                         const std::string format = "")
{
    using namespace utils::time;

    const auto nowTimeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const auto nowLocal = std::localtime(&nowTimeT);
    nowLocal->tm_hour   = hours.count();
    nowLocal->tm_min    = minutes.count();

    auto timestamp = TimestampFactory().createTimestamp(type, std::mktime(nowLocal));
    return timestamp->str(format);
}

inline TimePoint TimePointFromString(const char *s1)
{
    TimePoint tp;
    std::istringstream(s1) >> date::parse("%F %T", tp);
    return tp;
}

inline calendar::YearMonthDay TimePointToYearMonthDay(const TimePoint &tp)
{
    return date::year_month_day{date::floor<date::days>(tp)};
}

inline TimePoint TimePointFromYearMonthDay(const calendar::YearMonthDay &ymd)
{
    return date::sys_days{ymd.year() / ymd.month() / ymd.day()};
}

inline time_t TimePointToMin(const TimePoint &tp)
{
    auto time     = TimePointToTimeT(tp);
    auto duration = utils::time::Duration(time);
    auto minutes  = duration.getMinutes();
    return minutes;
}

inline uint32_t TimePointToHour12H(const TimePoint &tp)
{
    auto time = TimePointToTimeT(tp);
    calendar::Timestamp timestamp(time);
    auto hour = timestamp.get_date_time_sub_value(calendar::Timestamp::GetParameters::Hour);
    if (hour > 12) {
        hour -= 12;
    }
    return hour;
}

inline std::string TimePointToHourString12H(const TimePoint &tp)
{
    auto hour =
        calendar::Timestamp(TimePointToTimeT(tp)).get_UTC_date_time_sub_value(calendar::Timestamp::GetParameters::Hour);
    auto hour12h = date::make12(std::chrono::hours(hour)).count();
    return utils::to_string(hour12h);
}

inline std::string TimePointToHourString24H(const TimePoint &tp)
{
    auto hour =
        calendar::Timestamp(TimePointToTimeT(tp)).get_UTC_date_time_sub_value(calendar::Timestamp::GetParameters::Hour);
    return utils::to_string(hour);
}

inline std::string TimePointToMinutesString(const TimePoint &tp)
{
    auto minute       = TimePointToMinutes(tp);
    auto minuteString = std::to_string(minute);
    if (minute < 10) {
        minuteString = "0" + minuteString;
    }
    return minuteString;
}

// 0: Monday, 1: Tuesday ... 6: Sunday
inline unsigned int WeekdayIndexFromTimePoint(const TimePoint &tp)
{
    auto ymw = date::year_month_weekday{std::chrono::floor<date::days>(tp)};
    return ymw.weekday().iso_encoding() - 1;
}

inline TimePoint TimePointFloorMinutes(const TimePoint &tp)
{
    return std::chrono::floor<std::chrono::minutes>(tp);
}

/// Returns TimePoint within 24h after relative time based on tp as source of daytime
inline TimePoint GetFollowingDayTime(const TimePoint &tp, const TimePoint &relativeTime)
{
    auto diff = relativeTime - tp;
    if (diff > std::chrono::hours{0}) {
        return tp + std::chrono::ceil<date::days>(diff);
    }
    else if (diff == std::chrono::hours{0}) {
        return tp + date::days{1};
    }
    else if (diff < std::chrono::hours{0} && std::chrono::abs(diff) > std::chrono::hours{24}) {
        return tp - std::chrono::floor<date::days>(std::chrono::abs(diff));
    }
    else {
        return tp;
    }
}

inline TimePoint nextTimePointFromHHMM(std::chrono::hours hours, std::chrono::minutes minutes, const TimePoint &from)
{
    const auto fromTimeT = std::chrono::system_clock::to_time_t(from);
    const auto fromLocal = std::localtime(&fromTimeT);

    fromLocal->tm_hour = hours.count();
    fromLocal->tm_min  = minutes.count();
    auto nextTime      = TimePointFloorMinutes(std::chrono::system_clock::from_time_t(std::mktime(fromLocal)));

    return GetFollowingDayTime(nextTime, from);
}

inline std::string createUID()
{
    constexpr uint32_t bufferLimit = 16;
    char Buffer[bufferLimit];
    utils::time::Timestamp timestamp(std::time(nullptr));
    std::string UID{timestamp.str("%Y%m%dT%H%M%S")};
    UID += '-';
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 100);
    sprintf(Buffer, "%d", distrib(gen));
    UID += Buffer;

    return UID;
}

#endif
// DATECOMMON_H
