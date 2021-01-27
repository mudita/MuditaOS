// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
/// beware time_t size might vary, as i.e. on linux it's long int (64b -> 8B != 4B)
/// this calss uses strftime to convert timestamp to text, but for UTF8 elements (mon,day) it uses our locale
/// as stdlib builtin locale doesn't provide way to substitute C-LOCALE

/// The most important classes in this module:
///@class Timestamp: gmt and local time calculations
///@class DateTime: date and time displayed format representation

///@typedef TimePoint: The reference point for time conversion

///@note The basic unit for calculating time is std::chrono::seconds{}


#include "time/time_locale.hpp"
#include <bsp/rtc/rtc.hpp>
#include <log/log.hpp>
#include <vector>
#include <string>
#include <module-utils/date/include/date/date.h>
#include <Utils.hpp>
#include <random>

namespace utils::time
{
    using Clock     = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using YearMonthDay     = date::year_month_day;
    using YearMonthDayLast = date::year_month_day_last;
    using HourMinuteSecond = date::hh_mm_ss<std::chrono::seconds>;

    inline constexpr utils::time::TimePoint TIME_POINT_INVALID = date::sys_days{date::January / 1 / 1970};
    namespace digits::number
    {
        inline constexpr auto hours        = 2;
        inline constexpr auto minutes      = 2;
        inline constexpr auto seconds      = 2;
        inline constexpr auto day          = 2;
        inline constexpr auto month        = 2;
        inline constexpr auto year         = 4;
    }

    inline constexpr auto max_month_day            = 31;
    inline constexpr auto minutes_in_quarter_hour  = 15;

    enum class GetParameters
    {
        Hour,
        Minute,
        Second,
        Day,
        Month,
        Year
    };

    // helper class to not put everything in time
    struct Localer
    {
        enum class Replacements
        {
            DayAbbrev,
            DayLong,
            MonthAbbrev,
            MonthLong,
            Timezone,
        };

        UTF8 get_replacement(Replacements val, const struct tm &timeinfo);
    };

    enum class TimestampType
    {
        useAsGMT,
        useAsLocaltime
    };

    class Duration; // fw decl

    class TimeConversion
    {
        ///@brief Implementation of methods witch are needed to convert time between 3 basic c++ time types (TimePoint, time_t, struct tm)
      public:
        static std::tm toTm(const std::time_t &t);
        static std::tm toTm(const utils::time::TimePoint &tp);

        static utils::time::TimePoint toTimePoint(const std::time_t &t);
        static utils::time::TimePoint toTimePoint(const std::tm &tm);

        static time_t toTime_t(const std::tm &tm);
        static time_t toTime_t(const utils::time::TimePoint &tp);
    };

    class ISysTime
    {
      protected:
        virtual std::pair<TimePoint, std::chrono::seconds> getCurrentTime() const          = 0;
        virtual utils::time::TimePoint localTimeFromGMTTime(const utils::time::TimePoint &gmtTp) const               = 0;
        virtual utils::time::TimePoint gmtTimeFromLocalTime(const utils::time::TimePoint &localTp) const             = 0;
    };

    class SysTimeBase : public ISysTime
    {
        ///@brief Used to calculate UTC +0 time, and the desired local time offset.
        ///       Defines methods that are input to the corresponding system calls.
        ///       The methods are called internally in the DateTime and Timestamp classes to convert the appropriate local time representation.

      protected:
        std::chrono::seconds getLocalGMTTimeOffsetFromGMT(const utils::time::TimePoint &gmtTp) const;
        std::chrono::seconds getLocalGMTTimeOffsetFromLocal(const utils::time::TimePoint &localTp) const;

      public:
        std::pair<TimePoint, std::chrono::seconds> getCurrentTime() const override;
        utils::time::TimePoint localTimeFromGMTTime(const utils::time::TimePoint &gmtTp) const override;
        utils::time::TimePoint gmtTimeFromLocalTime(const utils::time::TimePoint &localTp) const override;
    };

    class Timestamp : protected Localer
    {
      private:
        SysTimeBase sysTimeBase;
        utils::time::TimePoint gmtTime;

      protected:
        utils::time::TimePoint localTime;
        SysTimeBase &getSysTimeBase();
        /// only reformat on: new format
        std::string format;

      public:
        utils::time::TimePoint getGMTTime() const;
        utils::time::TimePoint getLocalTime() const;

        std::chrono::seconds getOffset() const;

        explicit operator UTF8 *() const;

        operator TimePoint();

        Timestamp();
        Timestamp(const utils::time::TimePoint &newGMT);
        Timestamp(const utils::time::TimePoint &newGMT, std::chrono::seconds offset);
        Timestamp(const utils::time::TimePoint &newTime, TimestampType type);
        Timestamp(const utils::time::TimePoint &newTime, TimestampType type, std::chrono::seconds offset);
        Timestamp(const utils::time::TimePoint &newGMT, const utils::time::TimePoint &newLocal);

        void setTime();
        void setTime(const utils::time::TimePoint &newGMT);
        void setTime(const utils::time::TimePoint &newGMT, std::chrono::seconds offset);
        void setTime(const utils::time::TimePoint &newTime, TimestampType type);
        void setTime(const utils::time::TimePoint &newTime, TimestampType type, std::chrono::seconds offset);
        void setTime(const utils::time::TimePoint &newGMT, const utils::time::TimePoint &newLocal);

        void set_format(std::string format);

        operator UTF8()
        {
            return str();
        }

        friend std::ostream &operator<<(std::ostream &os, Timestamp t)
        {
            os << t.str();
            return os;
        }

        explicit operator YearMonthDay();
        explicit operator HourMinuteSecond();

        explicit operator date::year();
        explicit operator date::month();
        explicit operator date::day();
        explicit operator std::chrono::hours();
        explicit operator std::chrono::minutes();
        explicit operator std::chrono::seconds();

        friend Duration operator-(const Timestamp &lhs, const Timestamp &rhs);
        friend Timestamp operator-(const Timestamp &lhs, const std::chrono::seconds &rhs);
        friend Timestamp operator+(const Timestamp &lhs, const std::chrono::seconds &rhs);
        friend Timestamp operator+(const std::chrono::seconds &lhs, const Timestamp &rhs);

        Timestamp &operator-=(const std::chrono::seconds &rhs);
        Timestamp &operator+=(const std::chrono::seconds &rhs);

        friend inline bool operator==(const Timestamp &lhs, const Timestamp &rhs)
        {
            return lhs.getLocalTime() == rhs.getLocalTime();
        }
        friend inline bool operator!=(const Timestamp &lhs, const Timestamp &rhs)
        {
            return lhs.getLocalTime() != rhs.getLocalTime();
        }
        friend inline bool operator<(const Timestamp &lhs, const Timestamp &rhs)
        {
            return lhs.getLocalTime() < rhs.getLocalTime();
        }
        friend inline bool operator>(const Timestamp &lhs, const Timestamp &rhs)
        {
            return lhs.getLocalTime() > rhs.getLocalTime();
        }
        friend inline bool operator<=(const Timestamp &lhs, const Timestamp &rhs)
        {
            return lhs.getLocalTime() <= rhs.getLocalTime();
        }
        friend inline bool operator>=(const Timestamp &lhs, const Timestamp &rhs)
        {
            return lhs.getLocalTime() >= rhs.getLocalTime();
        }

        /// get Time in any format possible via strftime
        virtual UTF8 str(std::string format = "");

        /// get day UTF8 value
        UTF8 day(bool abbrev = false);

        /// get month UTF8 value
        UTF8 month(bool abbrev = false);

        UTF8 get_date_time_substr(GetParameters param);
        uint32_t get_date_time_sub_value(GetParameters param);
    };

    /// helper class to operate on time now
    /// takes timestamp and can show time in past
    class DateTime : public Timestamp
    {
      public:
        bool show_textual_past      = true;
        bool date_format_long       = true;
        bool time_format_24h        = true;
        std::string today_format    = "%H:%M";
        std::string long_ago_format = "%d.%m.%y";

        DateTime(bool date_format_long = true);

        DateTime(int i)      = delete;
        DateTime(uint32_t i) = delete;
        DateTime(time_t i)   = delete;

        DateTime(const utils::time::TimePoint &tp, bool date_format_long = true);

        friend std::ostream &operator<<(std::ostream &os, DateTime t)
        {
            os << t.str();
            return os;
        }

        /// Time have str(std::string ) this one uses presets
        virtual UTF8 str(std::string fmt = "");
        bool isToday();
        bool isYesterday();
    };

    class Date : public DateTime
    {
      public:
        Date(bool date_format_long = true) : DateTime(date_format_long){};
        Date(int i)      = delete;
        Date(uint32_t i) = delete;
        Date(time_t i)   = delete;
        Date(const utils::time::TimePoint &tp, bool date_format_long = true) : DateTime(tp, date_format_long){};
        virtual UTF8 str(std::string format = "") final;

        operator YearMonthDay();
    };

    class Time : public DateTime
    {
      public:
        Time(bool date_format_long = true) : DateTime(date_format_long){};
        Time(int i)      = delete;
        Time(uint32_t i) = delete;
        Time(time_t i)   = delete;
        Time(const utils::time::TimePoint &tp, bool date_format_long = true) : DateTime(tp, date_format_long){};
        virtual UTF8 str(std::string format = "") final;

        operator HourMinuteSecond();

        /// set time zone offset including adjustment for daylight saving
        static void setTimeZoneOffset(int tzOffset);
        /// get time zone offset including adjustment for daylight saving
        static int getTimeZoneOffset();
    };

    class Duration
    {
      public:
        /// Duration display format is generally in a form of hh:mm::ss or mm::ss with and without leading 0s
        /// @note hh may exceed 24, mm may exceed 60
        ///
        /// All the predefined format are stored in language json files.
        /// They are configurable by string replacement specifiers with (e.g. %0H) and without (e.g. %H) leading 0s.
        /// * %H / %0H - hours
        /// * %M / %0M - minutes
        /// * %N / %0N - hours and minutes (in minutes)
        /// * %S / %0S - seconds

        enum class DisplayedFormat
        {
            Fixed0M0S,  /// fixed format                          e.g.   00:00,   01:01,   12:12,   89:01,  1643:14
            FixedH0M0S, /// fixed format                          e.g. 0:00:00, 0:01:01, 0:12:12, 1:29:01, 27:23:14
            AutoM,      /// auto format without leading 0 in min  e.g.    0:00,    1:01,   12:12, 1:29:01, 27:23:14
            Auto0M      /// auto format with leading 0 in min     e.g.   00:00,   01:01,   12:12, 1:29:01, 27:23:14
        };

        explicit constexpr Duration(std::chrono::seconds duration = std::chrono::seconds(0)): duration(duration)
        {
            calculate();
        };

        Duration(const utils::time::TimePoint &stop, const utils::time::TimePoint &start);
        Duration(const Timestamp &stop, const Timestamp &start);

        std::chrono::seconds get() const;
        std::chrono::minutes getMinutes() const;
        std::chrono::hours getHours() const;
        date::days getRoundedDays() const;

        UTF8 str(DisplayedFormat displayedFormat = DisplayedFormat::Auto0M) const;

        // uses default format
        friend inline std::ostream &operator<<(std::ostream &os, Duration t)
        {
            os << t.str();
            return os;
        }

        operator std::chrono::seconds()
        {
            return duration;
        }

        friend Duration operator-(const Timestamp &lhs, const Timestamp &rhs);
        friend Timestamp operator-(const Timestamp &lhs, const std::chrono::seconds &rhs);
        friend Timestamp operator+(const Timestamp &lhs, const std::chrono::seconds &rhs);
        friend Timestamp operator+(const std::chrono::seconds &lhs, const Timestamp &rhs);

        friend inline Duration operator+(const Duration &lhs, const Duration &rhs)
        {
            return Duration(lhs.duration + rhs.duration);
        }
        friend inline Duration operator-(const Duration &lhs, const Duration &rhs)
        {
            return Duration(lhs.duration > rhs.duration ? lhs.duration - rhs.duration : std::chrono::seconds(0));
        }
        friend inline bool operator==(const Duration &lhs, const Duration &rhs)
        {
            return lhs.duration == rhs.duration;
        }
        friend inline bool operator!=(const Duration &lhs, const Duration &rhs)
        {
            return lhs.duration != rhs.duration;
        }
        friend inline bool operator<(const Duration &lhs, const Duration &rhs)
        {
            return lhs.duration < rhs.duration;
        }
        friend inline bool operator>(const Duration &lhs, const Duration &rhs)
        {
            return lhs.duration > rhs.duration;
        }
        friend inline bool operator<=(const Duration &lhs, const Duration &rhs)
        {
            return lhs.duration <= rhs.duration;
        }
        friend inline bool operator>=(const Duration &lhs, const Duration &rhs)
        {
            return lhs.duration >= rhs.duration;
        }

      private:
        static constexpr auto verboseConversion   = false; // debug switch

        void fillStr(std::string &formatlong) const;

        void constexpr calculate()
        {
            _hours     = std::chrono::duration_cast<std::chrono::hours>(this->duration);
            _hmminutes = std::chrono::duration_cast<std::chrono::minutes>(this->duration);
            _minutes   = std::chrono::duration_cast<std::chrono::minutes>(this->duration - std::chrono::duration_cast<std::chrono::hours>(this->duration));
            _seconds   = std::chrono::duration_cast<std::chrono::seconds>(this->duration - std::chrono::duration_cast<std::chrono::minutes>(this->duration));

            if (verboseConversion) {
                LOG_DEBUG("duration %" PRIu64 " - %d hours %d minutes %d seconds",
                          duration.count(),
                          static_cast<int>(_hours.count()),
                          static_cast<int>(_minutes.count()),
                          static_cast<int>(_seconds.count()));
            }
        };

        std::chrono::seconds duration   = std::chrono::seconds(0);
        std::chrono::hours _hours       = std::chrono::hours(0);
        std::chrono::minutes _hmminutes = std::chrono::minutes(0);
        std::chrono::minutes _minutes   = std::chrono::minutes(0);
        std::chrono::seconds _seconds   = std::chrono::seconds(0);
        date::days _roundedDays         = date::days{0};
    };

    class CalendarConversion
    {
        ///@brief Implementation of methods for date and time conversion witch are not implemented in TimeConversion class
        ///       Mostly date conversion used in Calendar Application
      public:
        static std::string TimePointToString(const utils::time::TimePoint &tp);
        static utils::time::TimePoint TimePointFromString(const char *s1);

        static utils::time::HourMinuteSecond TimePointToHourMinSec(const utils::time::TimePoint &tp);

        // 0: Monday, 1: Tuesday ... 6: Sunday
        static unsigned int WeekdayIndexFromTimePoint(const utils::time::TimePoint &tp);

        static utils::time::TimePoint getFirstWeekDay(const utils::time::TimePoint &tp);
        static std::string TimePointToString(const utils::time::TimePoint &tp, date::months months);
        static utils::time::TimePoint TimePointAddMonths(const utils::time::TimePoint &tp, date::months months);

        static std::string TimePointToLocalizedDateString(const utils::time::TimePoint &tp, const std::string format = "");
        static std::string TimePointToLocalizedTimeString(const utils::time::TimePoint &tp, const std::string format = "");

        static utils::time::YearMonthDay TimePointToYearMonthDay(const utils::time::TimePoint &tp);
        static utils::time::TimePoint TimePointFromYearMonthDay(const utils::time::YearMonthDay &ymd);

        static uint32_t TimePointToHour24H(const utils::time::TimePoint &tp);
        static uint32_t TimePointToHour12H(const utils::time::TimePoint &tp);

        static std::string TimePointToHourString24H(const utils::time::TimePoint &tp);
        static std::string TimePointToHourString12H(const utils::time::TimePoint &tp);
        static std::string TimePointToMinutesString(const utils::time::TimePoint &tp);

        static std::string createUID();
    };

    Timestamp getCurrentTimestamp();
    utils::time::TimePoint TimePointNow();

} // namespace utils
