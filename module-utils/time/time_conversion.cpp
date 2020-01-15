#include <algorithm>
#include <array>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <time.h>
#include <iomanip>
#include <locale>
#include <i18/i18.hpp>

#include "time_conversion.hpp"
#include "time_locale.hpp"

namespace utils {
namespace time {

Locale tlocale;

UTF8 Localer::get_replacement(Replacements val, const struct tm &timeinfo)
{
    UTF8 retval = "";
    switch (val) {
    case DayLong:
        retval = Locale::get_day(Locale::Day(timeinfo.tm_wday - 1));
        break;
    case DayAbbrev:
        retval = Locale::get_day(Locale::Day(timeinfo.tm_wday - 1)).substr(0,abbrev_len);
        break;
    case MonthLong:
        retval = Locale::get_month(Locale::Month(timeinfo.tm_mon));
        break;
    case MonthAbbrev:
        retval = Locale::get_month(Locale::Month(timeinfo.tm_mon)).substr(0,abbrev_len);
        break;
    case Timezone:
        break;
    }
    return retval;
}


void Time::replace_specifiers()
{
        int replacements_n = 0;
        const int strtof_formatter_size = 2;
        for (auto &el : specifiers_replacement) {
            auto begin_pos = format.begin();
            auto found_pos = format.begin();
            while (found_pos != format.end()) {
                begin_pos = format.begin();
                found_pos = std::search(begin_pos, format.end(), el.begin(), el.end());
                if (found_pos != format.end()) {
                    UTF8 begin = std::string(begin_pos, found_pos);
                    UTF8 day = get_replacement(Replacements(replacements_n), timeinfo);
                    UTF8 next;
                    // std::next doesnt care for distance... (doesn't return element.end()) need to check it
                    if (std::distance(found_pos, format.end()) <= strtof_formatter_size) {
                        next = "";
                    } else {
                        next = std::string(std::next(found_pos, strtof_formatter_size), format.end());
                    }
                    format = (begin + day + next).c_str();
                }
            }
            ++replacements_n;
        }
    }

    void Time::set_time(time_t newtime)
    {
        time = newtime;
        timeinfo = *localtime(&time);
    }

    void Time::set_time(std::string timestr, const char *format)
    {

        std::stringstream stream(timestr);
        try
		{
            stream >> std::get_time(&(this->timeinfo), "%y/%m/%d %H:%M:%S");
#ifdef TARGET_RT1051
            timeinfo.tm_year += 100;
#endif
            this->time = mktime(&timeinfo);
        }
		catch (std::exception &e)
		{
			LOG_ERROR("Time::set_time error %s", e.what());
		}
    }
    UTF8 Time::str(std::string format)
    {
        if(format.compare("") !=0) this->format = format;
        UTF8 datetimestr = "";
        replace_specifiers();
        std::strftime((char *)data, datasize, this->format.c_str(), &timeinfo);
        datetimestr = data;
        return datetimestr;
    }

    UTF8 Time::day(bool abbrev)
    {
        if(abbrev) {
            return get_replacement(Replacements::DayAbbrev, timeinfo);
        } else {
            return get_replacement(Replacements::DayLong, timeinfo);
        }
    }


    UTF8 Time::month(bool abbrev)
    {
        if(abbrev) {
            return get_replacement(Replacements::MonthAbbrev, timeinfo);
        } else {
            return get_replacement(Replacements::MonthLong, timeinfo);
        }
    }

    void SysTime::before_n_sec(time_t val)
    {
        ref_time = time;
        if(val) {
            set_time(val);
        }
    }

    UTF8 SysTime::str(std::string format)
    {
        auto newer_timeinfo = *localtime(&ref_time);
        const unsigned int seconds_in_2days = 3600 * 24 * 2;
        bool is_leap_year = (timeinfo.tm_year % 4 == 0 && timeinfo.tm_year % 100 != 0) || timeinfo.tm_year % 400 == 0;
        if(format.compare("") != 0) {
            return Time::str(format);
        }
        if( newer_timeinfo.tm_yday - timeinfo.tm_yday == 0) {
            return Time::str(Locale::format(Locale::Format12HourMin));
        } else if( show_textual_past
                && ((newer_timeinfo.tm_yday - timeinfo.tm_yday == 1)                            // day difference
                    ||(timeinfo.tm_year == 0 && newer_timeinfo.tm_year + 364 + is_leap_year)    // day next year day difference
                   )
                ) {
            return Locale::yesterday();
        } else {
            return Time::str(Locale::format(date_format_long ? Locale::FormatLocaleDateFull : Locale::FormatLocaleDateShort));
        }
    }


}; //time
}; // utils
