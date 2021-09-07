// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
/// beware time_t size might vary, as i.e. on linux it's long int (64b -> 8B != 4B)
/// this calss uses strftime to convert timestamp to text, but for UTF8 elements (mon,day) it uses our locale
/// as stdlib builtin locale doesn't provide way to substitute C-LOCALE

#include "time/time_locale.hpp"
#include <log.hpp>

#include <vector>
#include <string>

namespace utils
{
    namespace time
    {
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
            };

            UTF8 get_replacement(Replacements val, const struct tm &timeinfo) const;
        };

        class Duration; // fw decl

        class TimeSettingsInterface
        {
          public:
            virtual ~TimeSettingsInterface()
            {}

            virtual bool isTimeFormat12h() const  = 0;
            virtual bool isDateFormatDDMM() const = 0;
        };

        class Timestamp : protected Localer
        {
          protected:
            time_t time = 0;
            /// only reformat on: new format
            std::string format = "";

            explicit operator UTF8 *() const
            {
                return nullptr;
            }

          public:
            explicit Timestamp(time_t newtime = 0);
            virtual ~Timestamp() = default;

            void set_time(time_t newtime);
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
            virtual UTF8 str(std::string format = "") const;

            /// get day UTF8 value
            UTF8 day(bool abbrev = false) const;

            /// get month UTF8 value
            UTF8 month(bool abbrev = false) const;

            /// get timestamp value
            time_t getTime() const // TODO: alek: rem
            {
                return time;
            };
        };

        /// helper class to operate on time now
        /// takes timestamp and can show time in past
        class DateTime : public Timestamp
        {
            time_t referenceTime = 0;

          protected:
            const TimeSettingsInterface &timeSettings;

          public:
            /// shows time in past in relation to reference value
            ///
            /// @val - timestamp to show
            /// @reference - reference timestamp
            /// @timeSettings - time settings interface
            DateTime(const TimeSettingsInterface &timeSettings, time_t val, time_t reference = std::time(nullptr))
                : Timestamp(val), referenceTime(reference), timeSettings(timeSettings)
            {}

            friend std::ostream &operator<<(std::ostream &os, DateTime t)
            {
                os << t.str();
                return os;
            }

            UTF8 str(std::string fmt = "") const override;

            bool isToday() const;
            bool isYesterday() const;
            bool isCurrentYear() const;
        };

        class Date : public DateTime
        {
          public:
            explicit Date(const TimeSettingsInterface &timeSettings, time_t val = 0) : DateTime(timeSettings, val){};
            UTF8 str(std::string format = "") const final;
        };

        class DateText : public DateTime
        {
          public:
            explicit DateText(const TimeSettingsInterface &timeSettings, time_t val = 0)
                : DateTime(timeSettings, val){};
            UTF8 str(std::string format = "") const final;
        };

        class Time : public DateTime
        {
          public:
            explicit Time(const TimeSettingsInterface &timeSettings, time_t val = 0) : DateTime(timeSettings, val){};
            UTF8 str(std::string format = "") const final;
        };

        class Clock : public DateTime
        {
          public:
            explicit Clock(const TimeSettingsInterface &timeSettings, time_t val = 0) : DateTime(timeSettings, val){};
            UTF8 str(std::string format = "") const final;
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
