// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
/// beware time_t size might vary, as i.e. on linux it's long int (64b -> 8B != 4B)
/// this calss uses strftime to convert timestamp to text, but for UTF8 elements (mon,day) it uses our locale
/// as stdlib builtin locale doesn't provide way to substitute C-LOCALE

#include "time/time_locale.hpp"
#include <bsp/rtc/rtc.hpp>
#include <log/log.hpp>

#include <vector>
#include <string>

namespace utils
{
    namespace time
    {
        inline constexpr auto secondsInMinute        = 60;
        inline constexpr auto minutesInHour          = 60;
        inline constexpr auto hoursInday             = 24;
        inline constexpr auto minutesInQuarterOfHour = 15;
        inline constexpr auto secondsInHour          = minutesInHour * secondsInMinute;
        inline constexpr auto secondsInDay           = hoursInday * secondsInHour;
        inline constexpr auto milisecondsInSecond    = 1000;
        inline constexpr auto never_happens_value    = 0xFFFF;

        enum class GetParameters
        {
            Hour,
            Minute,
            Day,
            Month,
            Year
        };

        // helper class to not put everything in time
        struct Localer
        {
            /// see specifiers_replacements description above
            enum Replacements
            {
                DayAbbrev,
                DayLong,
                MonthAbbrev,
                MonthLong,
                Timezone,
            };

            UTF8 get_replacement(Replacements val, const struct tm &timeinfo);
        };

        class Duration; // fw decl

        class Timestamp : protected Localer
        {
          protected:
            time_t time = 0;
            struct tm timeinfo;
            /// only reformat on: new format
            std::string format = "";

            explicit operator UTF8 *() const
            {
                return nullptr;
            }

          public:
            Timestamp();
            Timestamp(time_t newtime) : time(newtime)
            {
                timeinfo = *localtime(&time);
            }

            /// set Time time_t value held (set timestamp)
            void set_time(time_t newtime);
            /// set Time from string
            void set_time(std::string time, const char *fmt);
            void set_format(std::string format)
            {
                this->format = format;
            }

            operator UTF8()
            {
                return str();
            }
            friend std::ostream &operator<<(std::ostream &os, Timestamp t)
            {
                os << t.str();
                return os;
            }
            friend Duration operator-(const Timestamp &lhs, const Timestamp &rhs);
            friend Timestamp operator-(const Timestamp &lhs, const Duration &rhs);
            friend Timestamp operator+(const Timestamp &lhs, const Duration &rhs);
            friend Timestamp operator+(const Duration &lhs, const Timestamp &rhs);

            friend inline bool operator==(const Timestamp &lhs, const Timestamp &rhs)
            {
                return lhs.time == rhs.time;
            }
            friend inline bool operator!=(const Timestamp &lhs, const Timestamp &rhs)
            {
                return lhs.time != rhs.time;
            }
            friend inline bool operator<(const Timestamp &lhs, const Timestamp &rhs)
            {
                return lhs.time < rhs.time;
            }
            friend inline bool operator>(const Timestamp &lhs, const Timestamp &rhs)
            {
                return lhs.time > rhs.time;
            }
            friend inline bool operator<=(const Timestamp &lhs, const Timestamp &rhs)
            {
                return lhs.time <= rhs.time;
            }
            friend inline bool operator>=(const Timestamp &lhs, const Timestamp &rhs)
            {
                return lhs.time >= rhs.time;
            }

            /// get Time in any format possible via strftime
            virtual UTF8 str(std::string format = "");

            /// get day UTF8 value
            UTF8 day(bool abbrev = false);

            /// get month UTF8 value
            UTF8 month(bool abbrev = false);

            // get timestamp value
            time_t getTime() const
            {
                return time;
            };

            UTF8 get_date_time_substr(GetParameters param);
            uint32_t get_date_time_sub_value(GetParameters param);
            uint32_t get_UTC_date_time_sub_value(GetParameters param);
        };

        /// helper class to operate on time now
        /// takes timestamp and can show time in past
        class DateTime : public Timestamp
        {
            time_t local_time = 0;

          public:
            bool show_textual_past      = true;
            bool date_format_long       = true;
            std::string today_format    = "%H:%M";
            std::string long_ago_format = "%d.%m.%y";

            /// shows time in past: time_now - val in seconds
            DateTime(time_t val = 0, bool date_format_long = true) : date_format_long(date_format_long)
            {
                before_n_sec(val);
            }

            friend std::ostream &operator<<(std::ostream &os, DateTime t)
            {
                os << t.str();
                return os;
            }

            /// converter -> returns time in past: (val) and stores localtime in ref_time
            void before_n_sec(time_t val);

            /// Time have str(std::string ) this one uses presets
            virtual UTF8 str(std::string fmt = "");
            bool isToday();
            bool isYesterday();
        };

        class Date : public DateTime
        {
          public:
            Date(time_t val = 0, bool date_format_long = true) : DateTime(val, date_format_long){};
            virtual UTF8 str(std::string format = "") final;
        };

        class Time : public DateTime
        {
          public:
            Time(time_t val = 0, bool date_format_long = true) : DateTime(val, date_format_long){};
            virtual UTF8 str(std::string format = "") final;

            /// set time zone offset including adjustment for daylight saving
            static void setTimeZoneOffset(int tzOffset);
            // get time zone offset including adjustment for daylight saving
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

            Duration(time_t duration = 0);
            Duration(time_t stop, time_t start);
            Duration(const Timestamp &stop, const Timestamp &start);

            time_t get() const
            {
                return duration;
            }

            time_t getMinutes() const
            {
                return minutes;
            }

            time_t getHours() const
            {
                return hours;
            }

            UTF8 str(DisplayedFormat displayedFormat = DisplayedFormat::Auto0M) const;

            // uses default format
            friend inline std::ostream &operator<<(std::ostream &os, Duration t)
            {
                os << t.str();
                return os;
            }

            friend Timestamp operator-(const Timestamp &lhs, const Duration &rhs);
            friend Timestamp operator+(const Timestamp &lhs, const Duration &rhs);
            friend Timestamp operator+(const Duration &lhs, const Timestamp &rhs);

            friend inline Duration operator+(const Duration &lhs, const Duration &rhs)
            {
                return Duration(lhs.duration + rhs.duration);
            }
            friend inline Duration operator-(const Duration &lhs, const Duration &rhs)
            {
                return Duration(lhs.duration > rhs.duration ? lhs.duration - rhs.duration : 0);
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
            void fillStr(std::string &formatlong) const;
            void calculate();
            time_t duration         = 0;
            unsigned long hours     = 0;
            unsigned long hmminutes = 0;
            unsigned long minutes   = 0;
            unsigned long seconds   = 0;
        };

        Timestamp getCurrentTimestamp();
    } // namespace time
} // namespace utils
