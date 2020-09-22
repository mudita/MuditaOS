#ifndef DATECOMMON_H
#define DATECOMMON_H

#include <module-utils/date/include/date/date.h>
#include <time/time_conversion.hpp>

using namespace std::chrono;

using Clock            = system_clock;
using TimePoint        = time_point<Clock>;
using YearMonthDay     = date::year_month_day;
using YearMonthDayLast = date::year_month_day_last;

const inline int max_month_day = 48;

enum class Reminder
{
    never,
    event_time,
    five_min_before,
    fifteen_min_before,
    thirty_min_before,
    one_hour_before,
    two_hour_before,
    one_day_before,
    two_days_before,
    one_week_before
};

enum class Repeat
{
    never,
    daily,
    weekly,
    two_weeks,
    month,
    year,
    custom
};

constexpr TimePoint TIME_POINT_INVALID = TimePoint::min();

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
    return system_clock::from_time_t(time);
}

inline TimePoint TimePointNow()
{
    utils::time::Timestamp timestamp;
    return TimePointFromTimeT(timestamp.getTime());
}

inline std::string TimePointToString(const TimePoint &tp)
{
    return date::format("%F %T", time_point_cast<seconds>(tp));
}

inline std::string TimePointToString(const TimePoint &tp, date::months months)
{
    date::year_month_day yearMonthDay     = date::year_month_day{date::floor<date::days>(tp)};
    date::year_month_day yearMonthDayLast = yearMonthDay.year() / yearMonthDay.month() / date::last;

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
    return date::format("%F %T", time_point_cast<seconds>(timePoint));
}

inline TimePoint TimePointFromString(const char *s1)
{
    TimePoint tp;
    std::istringstream(s1) >> date::parse("%F %T", tp);
    return tp;
}

inline time_t TimePointToTimeT(const TimePoint &tp)
{
    return system_clock::to_time_t(tp);
}

inline YearMonthDay TimePointToYearMonthDay(const TimePoint &tp)
{
    return date::year_month_day{date::floor<date::days>(tp)};
}

inline TimePoint TimePointFromYearMonthDay(const YearMonthDay &ymd)
{
    return date::sys_days{ymd.year() / ymd.month() / ymd.day()};
}

inline auto TimePointToHourMinSec(const TimePoint &tp)
{
    auto dp = date::floor<date::days>(tp);
    return date::make_time(tp - dp);
}

// 0: Monday, 1: Tuesday ... 6: Sunday
inline unsigned int WeekdayIndexFromTimePoint(const TimePoint &tp)
{
    auto ymw = date::year_month_weekday{floor<date::days>(tp)};
    return ymw.weekday().iso_encoding() - 1;
}

#endif
// DATECOMMON_H
