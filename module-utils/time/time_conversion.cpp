#include <algorithm>
#include <array>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <locale>
#include <i18/i18.hpp>

#include <log/log.hpp>
#include "time_conversion.hpp"
#include "time_locale.hpp"
#include <Utils.hpp>

namespace utils
{
    namespace time
    {

        Locale tlocale;

        UTF8 Localer::get_replacement(Replacements val, const struct tm &timeinfo)
        {
            UTF8 retval = "";
            switch (val) {
            case DayLong:
                retval = Locale::get_day(Locale::Day(timeinfo.tm_wday - 1));
                break;
            case DayAbbrev:
                retval = Locale::get_day(Locale::Day(timeinfo.tm_wday - 1)).substr(0, abbrev_len);
                break;
            case MonthLong:
                retval = Locale::get_month(Locale::Month(timeinfo.tm_mon));
                break;
            case MonthAbbrev:
                retval = Locale::get_month(Locale::Month(timeinfo.tm_mon)).substr(0, abbrev_len);
                break;
            case Timezone:
                break;
            }
            return retval;
        }

        void Timestamp::replace_specifiers()
        {
            int replacements_n              = 0;
            const int strtof_formatter_size = 2;
            for (auto &el : specifiers_replacement) {
                auto begin_pos = format.begin();
                auto found_pos = format.begin();
                while (found_pos != format.end()) {
                    begin_pos = format.begin();
                    found_pos = std::search(begin_pos, format.end(), el.begin(), el.end());
                    if (found_pos != format.end()) {
                        UTF8 begin = std::string(begin_pos, found_pos);
                        UTF8 day   = get_replacement(Replacements(replacements_n), timeinfo);
                        UTF8 next;
                        // std::next doesnt care for distance... (doesn't return element.end()) need to check it
                        if (std::distance(found_pos, format.end()) <= strtof_formatter_size) {
                            next = "";
                        }
                        else {
                            next = std::string(std::next(found_pos, strtof_formatter_size), format.end());
                        }
                        format = (begin + day + next).c_str();
                    }
                }
                ++replacements_n;
            }
        }

        void Timestamp::set_time(time_t newtime)
        {
            time     = newtime;
            timeinfo = *localtime(&time);
        }

        void Timestamp::set_time(std::string timestr, const char *format)
        {

            std::stringstream stream(timestr);
            try {
                stream >> std::get_time(&(this->timeinfo), "%y/%m/%d %H:%M:%S");

                // add missing years, otherwise mktime returns bad timestamp
                timeinfo.tm_year += 100;
                this->time = mktime(&timeinfo);
            }
            catch (std::exception &e) {
                LOG_ERROR("Time::set_time error %s", e.what());
            }
        }

        constexpr uint32_t datasize = 128;
        UTF8 Timestamp::str(std::string format)
        {
            if (format.compare("") != 0)
                this->format = format;
            UTF8 datetimestr = "";
            replace_specifiers();

            auto data = std::unique_ptr<char[]>(new char[datasize]);
            std::strftime(data.get(), datasize, this->format.c_str(), &timeinfo);
            datetimestr = data.get();
            return datetimestr;
        }

        UTF8 Timestamp::day(bool abbrev)
        {
            if (abbrev) {
                return get_replacement(Replacements::DayAbbrev, timeinfo);
            }
            else {
                return get_replacement(Replacements::DayLong, timeinfo);
            }
        }

        UTF8 Timestamp::month(bool abbrev)
        {
            if (abbrev) {
                return get_replacement(Replacements::MonthAbbrev, timeinfo);
            }
            else {
                return get_replacement(Replacements::MonthLong, timeinfo);
            }
        }

        Duration operator-(const Timestamp &lhs, const Timestamp &rhs)
        {
            return Duration(lhs.time, rhs.time);
        };

        Timestamp operator+(const Timestamp &lhs, const Duration &rhs)
        {
            return Timestamp(lhs.time + rhs.get());
        };

        void DateTime::before_n_sec(time_t val)
        {
            local_time = time;
            if (val) {
                set_time(val);
            }
        }

        bool DateTime::isToday()
        {
            auto newer_timeinfo = *localtime(&local_time);
            return (newer_timeinfo.tm_yday == timeinfo.tm_yday && newer_timeinfo.tm_year == timeinfo.tm_year);
        }

        bool DateTime::isYesterday()
        {
            auto newer_timeinfo = *localtime(&local_time);
            bool is_leap_year =
                (timeinfo.tm_year % 4 == 0 && timeinfo.tm_year % 100 != 0) || timeinfo.tm_year % 400 == 0;

            return (((newer_timeinfo.tm_yday - timeinfo.tm_yday == 1) &&
                     (newer_timeinfo.tm_year == timeinfo.tm_year)) // day difference
                    || (timeinfo.tm_year == 0 &&
                        newer_timeinfo.tm_year + 364 + is_leap_year) // day next year day difference
            );
        }

        UTF8 DateTime::str(std::string format)
        {
            if (format.compare("") != 0) {
                return Timestamp::str(format);
            }
            if (isToday()) // if the same computer day, then return hour.
            {
                return Timestamp::str(Locale::format(Locale::FormatTime12H));
            }
            else if (show_textual_past && isYesterday()) {
                return Locale::yesterday();
            }
            else {
                return Timestamp::str(
                    Locale::format(date_format_long ? Locale::FormatLocaleDateFull : Locale::FormatLocaleDateShort));
            }
        }
        UTF8 Timestamp::get_date_time_substr(GetParameters param)
        {
            auto value = get_date_time_sub_value(param);
            if (value != UINT32_MAX) {
                return UTF8(std::to_string(value));
            }
            return UTF8();
        }

        uint32_t Timestamp::get_date_time_sub_value(GetParameters param)
        {
            switch (param) {
            case GetParameters::Hour:
                return timeinfo.tm_hour;
                break;
            case GetParameters::Minute:
                return timeinfo.tm_min;
                break;
            case GetParameters::Day:
                return timeinfo.tm_mday;
                break;
            case GetParameters::Month:
                return timeinfo.tm_mon + 1;
                break;
            case GetParameters::Year:
                return timeinfo.tm_year + 1900;
                break;
            }
            return UINT32_MAX;
        }
        UTF8 Date::str(std::string format)
        {
            if (!format.empty()) {
                return Timestamp::str(format);
            }
            else if (show_textual_past) {
                if (isToday()) {
                    return Locale::today();
                }
                else if (isYesterday()) {
                    return Locale::yesterday();
                }
            }

            return Timestamp::str(
                Locale::format(date_format_long ? Locale::FormatLocaleDateFull : Locale::FormatLocaleDateShort));
        }

        UTF8 Time::str(std::string format)
        {
            if (!format.empty()) {
                return Timestamp::str(format);
            }
            else {
                return Timestamp::str(
                    Locale::format(Locale::FormatTime12H)); // @TODO: M.G. FormatLocaleTime which actually works
            }
        }

        Duration::Duration(time_t duration, DisplayedFields displayedFields)
            : duration(duration), displayedFields(displayedFields)
        {
            days    = this->duration / secondsInDay;
            hours   = (this->duration % secondsInDay) / secondsInHour;
            minutes = ((this->duration % secondsInDay) % secondsInHour) / secondsInMinute;
            seconds = ((this->duration % secondsInDay) % secondsInHour) % secondsInMinute;

            if (verboseConversion) {
                LOG_DEBUG(
                    "durtaion %u = %u days %u hours %u minutes %u seconds", duration, days, hours, minutes, seconds);
            }
        }

        Duration::Duration(time_t stop, time_t start, DisplayedFields displayedFields)
            : displayedFields(displayedFields)
        {
            auto diff = stop - start;
            duration  = diff >= 0 ? diff : 0;
        }

        Duration::Duration(const Timestamp &stop, const Timestamp &start, DisplayedFields displayedFields)
            : Duration(stop.getTime(), start.getTime(), displayedFields)
        {}

        void Duration::fillStr(std::string &format) const
        {
            utils::findAndReplaceAll(format, "%d", std::to_string(days));
            utils::findAndReplaceAll(format, "%H", std::to_string(hours));
            utils::findAndReplaceAll(format, "%M", std::to_string(minutes));
            utils::findAndReplaceAll(format, "%S", std::to_string(seconds));
        }

        UTF8 Duration::str() const
        {
            auto data = displayedFields == DisplayedFields::three ? utils::localize.get(durationFormatHMS)
                                                                  : utils::localize.get(durationFormatMS);

            if (days != 0) {
                data = displayedFields == DisplayedFields::three ? utils::localize.get(durationFormatDHM)
                                                                 : utils::localize.get(durationFormatDH);
            }
            else if (hours != 0) {
                data = displayedFields == DisplayedFields::three ? utils::localize.get(durationFormatHMS)
                                                                 : utils::localize.get(durationFormatHM);
            }

            fillStr(data);

            return data;
        }

    }; // namespace time
};     // namespace utils
