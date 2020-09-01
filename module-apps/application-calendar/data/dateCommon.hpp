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
    two_weeks,
    month,
    year,
    custom
};

// mlucki
// constexpr TimePoint TIME_POINT_INVALID = TimePoint::min();
constexpr TimePoint TIME_POINT_INVALID = date::sys_days{date::January / 1 / 1970};

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

inline time_t TimePointToTimeT(const TimePoint &tp)
{
    return system_clock::to_time_t(tp);
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

inline auto TimePointToHourMinSec(const TimePoint &tp)
{
    auto dp = date::floor<date::days>(tp);
    return date::make_time(tp - dp);
}

inline uint32_t TimePointToHour24H(const TimePoint &tp)
{
    auto time = TimePointToTimeT(tp);
    utils::time::Timestamp timestamp(time);
    auto hour = timestamp.get_date_time_sub_value(utils::time::GetParameters::Hour);
    return hour;
}

inline uint32_t TimePointToMinutes(const TimePoint &tp)
{
    auto time = TimePointToTimeT(tp);
    utils::time::Timestamp timestamp(time);
    auto minute = timestamp.get_date_time_sub_value(utils::time::GetParameters::Minute);
    return minute;
}

inline TimePoint getFirstWeekDay(const TimePoint &tp)
{
    date::year_month_day yearMonthDay = date::year_month_day{date::floor<date::days>(tp)};
    auto hourV                        = TimePointToHour24H(tp);
    auto minuteV                      = TimePointToMinutes(tp);
    while (date::weekday{yearMonthDay} != date::mon) {
        auto decrementedDay = --yearMonthDay.day();
        yearMonthDay        = yearMonthDay.year() / yearMonthDay.month() / decrementedDay;
    }
    auto finalDate     = date::sys_days{yearMonthDay.year() / yearMonthDay.month() / yearMonthDay.day()};
    auto finalDateTime = finalDate + std::chrono::hours(hourV) + std::chrono::minutes(minuteV);

    return finalDateTime;
}

inline std::string TimePointToDateString(const TimePoint &tp)
{
    return date::format("%F", time_point_cast<seconds>(tp));
}

inline std::string TimePointToTimeString(const TimePoint &tp)
{
    return date::format("%T", time_point_cast<seconds>(tp));
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

inline YearMonthDay TimePointToYearMonthDay(const TimePoint &tp)
{
    return date::year_month_day{date::floor<date::days>(tp)};
}

inline TimePoint TimePointFromYearMonthDay(const YearMonthDay &ymd)
{
    return date::sys_days{ymd.year() / ymd.month() / ymd.day()};
}

inline time_t TimePointToMin(const TimePoint &tp)
{
    auto time     = TimePointToTimeT(tp);
    auto duration = new utils::time::Duration(time);
    auto minutes  = duration->getMinutes();
    return minutes;
}

inline uint32_t TimePointToHour12H(const TimePoint &tp)
{
    auto time = TimePointToTimeT(tp);
    utils::time::Timestamp timestamp(time);
    auto hour = timestamp.get_date_time_sub_value(utils::time::GetParameters::Hour);
    if (hour > 12) {
        hour -= 12;
    }
    return hour;
}

inline std::string TimePointToHourString12H(const TimePoint &tp)
{
    auto hour       = TimePointToHour12H(tp);
    auto hourString = std::to_string(hour);
    return hourString;
}

inline std::string TimePointToHourString24H(const TimePoint &tp)
{
    auto hour       = TimePointToHour24H(tp);
    auto hourString = std::to_string(hour);
    return hourString;
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
    auto ymw = date::year_month_weekday{floor<date::days>(tp)};
    return ymw.weekday().iso_encoding() - 1;
}

#endif
// DATECOMMON_H
