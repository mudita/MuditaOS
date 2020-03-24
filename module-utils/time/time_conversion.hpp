#pragma once
/// beware time_t size might vary, as i.e. on linux it's long int (64b -> 8B != 4B)
/// this calss uses strftime to convert timestamp to text, but for UTF8 elements (mon,day) it uses our locale
/// as stdlib builtin locale doesn't provide way to substitute C-LOCALE

#include "time/time_locale.hpp"
#include <bsp/rtc/rtc.hpp>
#include <vector>
#include <string>
#include <log/log.hpp>

namespace utils
{
    namespace time
    {

        constexpr auto secondsInMinute = 60;
        constexpr auto minutesInHour   = 60;
        constexpr auto hoursInday      = 24;
        constexpr auto secondsInHour   = minutesInHour * secondsInMinute;
        constexpr auto secondsInDay    = hoursInday * secondsInHour;

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
            const inline static unsigned int abbrev_len = 3;
            /// order matters, it's used in replace_locale with enum Replacements
            const inline static std::vector<std::string> specifiers_replacement = {"%a",  // day abbrew
                                                                                   "%A",  // day long
                                                                                   "%b",  // month abbrew
                                                                                   "%B",  // month long
                                                                                   "%Z"}; // timezone
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

            /// replace day mon specifiers (first 2 characters)
            /// cant use std::replace -> due to fact that it doesn't support multiple element replace (or i cant find
            /// it) cant use string::replace -> expcetion out_of_range on size when replacing with bigger (our case)
            /// please be vary when using begin_pos/found_pos (as format in next loops might be in totally different
            /// place)
            void replace_specifiers();

          public:
            Timestamp()
            {
                auto err = bsp::rtc_GetCurrentTimestamp(&time);
                if (err) {
                    LOG_ERROR("rtc_GetCurrentTimestamp failure!");
                }
                timeinfo = *localtime(&time);
            }
            Timestamp(time_t newtime) : time(newtime)
            {
                timeinfo = *localtime(&time);
            }

            /// set Time time_t value held (set timestamp)
            void set_time(time_t newtime);
            /// set Time from string
            void set_time(std::string time, const char *format);
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
            virtual UTF8 str(std::string format = "");
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
            static constexpr bool verboseConversion             = false; // debug switch
            static const inline std::string durationFormatH0M0S = "duration_hour_0min_0sec";
            static const inline std::string durationFormat0M0S  = "duration_0min_0sec";
            static const inline std::string durationFormat0N0S  = "duration_0hmin_0sec";
            static const inline std::string durationFormatM0S   = "duration_min_0sec";

            struct Format
            {
                const std::string lowFormat;  /// format used if duration is below 1 hour
                const std::string highFormat; /// format used if duration exceeds 1 hour
            };
            using FormatMap                         = std::map<const DisplayedFormat, const Format>;
            static const inline FormatMap formatMap = {
                {DisplayedFormat::Fixed0M0S, {durationFormat0N0S, durationFormat0N0S}},
                {DisplayedFormat::FixedH0M0S, {durationFormatH0M0S, durationFormatH0M0S}},
                {DisplayedFormat::AutoM, {durationFormatM0S, durationFormatH0M0S}},
                {DisplayedFormat::Auto0M, {durationFormat0M0S, durationFormatH0M0S}}};

            void fillStr(std::string &formatlong) const;
            void calculate();
            time_t duration         = 0;
            unsigned long hours     = 0;
            unsigned long hmminutes = 0;
            unsigned long minutes   = 0;
            unsigned long seconds   = 0;
        };

    } // namespace time
} // namespace utils
