// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "time_conversion.hpp"
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
#include "i18n/i18n.hpp"

#include "time_locale.hpp"
#include <Utils.hpp>
#include <time/time_constants.hpp>

namespace utils::time
{
    namespace
    {
        constexpr auto verboseConversion = false; // debug switch

        constexpr auto abbrev_len = 3U;

        /// order matters, it's used in replace_locale with enum Replacements
        const std::vector<std::string> specifiers_replacement = {"%a",  // day abbrew
                                                                 "%A",  // day long
                                                                 "%b",  // month abbrew
                                                                 "%B"}; // month long

        struct Format
        {
            const std::string lowFormat;  /// format used if duration is below 1 hour
            const std::string highFormat; /// format used if duration exceeds 1 hour
        };
        constexpr auto durationFormatH0M0S = "duration_hour_0min_0sec";
        constexpr auto durationFormat0M0S  = "duration_0min_0sec";
        constexpr auto durationFormat0N0S  = "duration_0hmin_0sec";
        constexpr auto durationFormatM0S   = "duration_min_0sec";

        using FormatMap           = std::map<const Duration::DisplayedFormat, const Format>;
        const FormatMap formatMap = {
            {Duration::DisplayedFormat::Fixed0M0S, {durationFormat0N0S, durationFormat0N0S}},
            {Duration::DisplayedFormat::FixedH0M0S, {durationFormatH0M0S, durationFormatH0M0S}},
            {Duration::DisplayedFormat::AutoM, {durationFormatM0S, durationFormatH0M0S}},
            {Duration::DisplayedFormat::Auto0M, {durationFormat0M0S, durationFormatH0M0S}}};
    } // namespace

    Locale tlocale;

    UTF8 Localer::get_replacement(Replacements val, const struct tm &timeinfo) const
    {
        UTF8 retval = "";
        switch (val) {
        case DayLong:
            retval = Locale::get_day(Locale::Day(timeinfo.tm_wday));
            break;
        case DayAbbrev:
            retval = Locale::get_day(Locale::Day(timeinfo.tm_wday)).substr(0, abbrev_len);
            break;
        case MonthLong:
            retval = Locale::get_month(Locale::Month(timeinfo.tm_mon));
            break;
        case MonthAbbrev:
            retval = Locale::get_month(Locale::Month(timeinfo.tm_mon)).substr(0, abbrev_len);
            break;
        }
        return retval;
    }

    Timestamp::Timestamp(time_t newtime) : time(newtime)
    {}

    void Timestamp::set_time(time_t newtime)
    {
        time = newtime;
    }
    constexpr uint32_t datasize = 128;
    UTF8 Timestamp::str(std::string fmt) const
    {
        if (fmt.compare("") == 0) {
            fmt = format; // take default
        }
        UTF8 datetimestr = "";
        auto timeInfo    = std::localtime(&time);
        auto replaceFunc = [&](int idx) { return get_replacement(Replacements(idx), *timeInfo); };
        utils::findAndReplaceAll(fmt, specifiers_replacement, replaceFunc);
        auto data = std::unique_ptr<char[]>(new char[datasize]);
        if (std::strftime(data.get(), datasize, fmt.c_str(), timeInfo) != 0) {
            datetimestr = UTF8(data.get());
        }
        return datetimestr;
    }

    UTF8 Timestamp::day(bool abbrev) const
    {
        auto timeInfo = *std::localtime(&time);
        if (abbrev) {
            return get_replacement(Replacements::DayAbbrev, timeInfo);
        }
        else {
            return get_replacement(Replacements::DayLong, timeInfo);
        }
    }

    UTF8 Timestamp::month(bool abbrev) const
    {
        auto timeInfo = *std::localtime(&time);
        if (abbrev) {
            return get_replacement(Replacements::MonthAbbrev, timeInfo);
        }
        else {
            return get_replacement(Replacements::MonthLong, timeInfo);
        }
    }

    Duration operator-(const Timestamp &lhs, const Timestamp &rhs)
    {
        return Duration(lhs.time, rhs.time);
    };
    Timestamp operator-(const Timestamp &lhs, const Duration &rhs)
    {
        return Timestamp(lhs.time < rhs.duration ? 0 : lhs.time - rhs.duration);
    };
    Timestamp operator+(const Timestamp &lhs, const Duration &rhs)
    {
        return Timestamp(lhs.time + rhs.duration);
    };
    Timestamp operator+(const Duration &lhs, const Timestamp &rhs)
    {
        return Timestamp(lhs.duration + rhs.time);
    }

    bool DateTime::isToday() const
    {
        auto timeinfo       = *std::localtime(&time);
        auto newer_timeinfo = *std::localtime(&referenceTime);
        return (newer_timeinfo.tm_yday == timeinfo.tm_yday && isCurrentYear());
    }

    bool DateTime::isYesterday() const
    {
        auto timeinfo       = *std::localtime(&time);
        auto newer_timeinfo = *std::localtime(&referenceTime);

        newer_timeinfo.tm_mday -= 1;
        std::mktime(&newer_timeinfo);

        return timeinfo.tm_year == newer_timeinfo.tm_year && timeinfo.tm_mon == newer_timeinfo.tm_mon &&
               timeinfo.tm_mday == newer_timeinfo.tm_mday;
    }

    bool DateTime::isCurrentYear() const
    {
        auto timeinfo       = *std::localtime(&time);
        auto newer_timeinfo = *std::localtime(&referenceTime);
        return (newer_timeinfo.tm_year == timeinfo.tm_year);
    }

    UTF8 DateTime::str(std::string format) const
    {
        if (format.compare("") != 0) {
            return Timestamp::str(format);
        }
        if (isToday()) {
            auto localeFormat =
                timeSettings.isTimeFormat12h() ? Locale::TimeFormat::FormatTime12H : Locale::TimeFormat::FormatTime24H;
            return Timestamp::str(Locale::format(localeFormat));
        }
        if (isYesterday()) {
            return Locale::yesterday();
        }
        if (isCurrentYear()) {
            auto localeFormat = timeSettings.isDateFormatDDMM() ? Locale::TimeFormat::FormatLocaleDate_DD_MM
                                                                : Locale::TimeFormat::FormatLocaleDate_MM_DD;
            return Timestamp::str(Locale::format(localeFormat));
        }
        auto localeFormat = timeSettings.isDateFormatDDMM() ? Locale::TimeFormat::FormatLocaleDate_DD_MM_YYYY
                                                            : Locale::TimeFormat::FormatLocaleDate_MM_DD_YYYY;
        return Timestamp::str(Locale::format(localeFormat));
    }

    UTF8 Date::str(std::string format) const
    {
        if (!format.empty()) {
            return Timestamp::str(format);
        }

        auto localeFormat = timeSettings.isDateFormatDDMM() ? Locale::TimeFormat::FormatLocaleDate_DD_MM_YYYY
                                                            : Locale::TimeFormat::FormatLocaleDate_MM_DD_YYYY;
        return Timestamp::str(Locale::format(localeFormat));
    }

    UTF8 DateText::str(std::string format) const
    {
        if (!format.empty()) {
            return Timestamp::str(format);
        }

        auto localeFormat = timeSettings.isDateFormatDDMM() ? Locale::TimeFormat::FormatDate_Day_DD_Month
                                                            : Locale::TimeFormat::FormatDate_Day_Month_DD;
        return Timestamp::str(Locale::format(localeFormat));
    }

    UTF8 Time::str(std::string format) const
    {
        if (!format.empty()) {
            return Timestamp::str(format);
        }
        auto localeFormat =
            timeSettings.isTimeFormat12h() ? Locale::TimeFormat::FormatTime12H : Locale::TimeFormat::FormatTime24H;
        return Timestamp::str(Locale::format(localeFormat));
    }

    UTF8 Clock::str(std::string format) const
    {
        if (!format.empty()) {
            return Timestamp::str(format);
        }
        auto localeFormat =
            timeSettings.isTimeFormat12h() ? Locale::TimeFormat::FormatTime12HShort : Locale::TimeFormat::FormatTime24H;
        return Timestamp::str(Locale::format(localeFormat));
    }

    Duration::Duration(time_t duration) : duration(duration)
    {
        calculate();
    }

    Duration::Duration(time_t stop, time_t start) : Duration(stop - start > 0 ? stop - start : 0)
    {}

    Duration::Duration(const Timestamp &stop, const Timestamp &start) : Duration(stop.getTime(), start.getTime())
    {}

    void Duration::fillStr(std::string &format) const
    {
        constexpr auto numberOfLeadingDigits = 2;
        utils::findAndReplaceAll(format, "%H", utils::to_string(hours));
        utils::findAndReplaceAll(format, "%M", utils::to_string(minutes));
        utils::findAndReplaceAll(format, "%N", utils::to_string(hmminutes));
        utils::findAndReplaceAll(format, "%S", utils::to_string(seconds));
        utils::findAndReplaceAll(format, "%0H", utils::addLeadingZeros(utils::to_string(hours), numberOfLeadingDigits));
        utils::findAndReplaceAll(
            format, "%0M", utils::addLeadingZeros(utils::to_string(minutes), numberOfLeadingDigits));
        utils::findAndReplaceAll(
            format, "%0N", utils::addLeadingZeros(utils::to_string(hmminutes), numberOfLeadingDigits));
        utils::findAndReplaceAll(
            format, "%0S", utils::addLeadingZeros(utils::to_string(seconds), numberOfLeadingDigits));
    }

    void Duration::calculate()
    {
        hours     = this->duration / secondsInHour;
        hmminutes = this->duration / secondsInMinute;
        minutes   = (this->duration % secondsInHour) / secondsInMinute;
        seconds   = (this->duration % secondsInHour) % secondsInMinute;

        if (verboseConversion) {
            LOG_DEBUG("duration %" PRIu64 " - %lu hours %lu minutes %lu seconds", duration, hours, minutes, seconds);
        }
    }

    UTF8 Duration::str(DisplayedFormat displayedFormat) const
    {
        // switch between format low and hig
        std::string data = utils::translate(hours != 0 ? formatMap.at(displayedFormat).highFormat
                                                       : formatMap.at(displayedFormat).lowFormat);
        fillStr(data);

        return data;
    }

    Timestamp getCurrentTimestamp()
    {
        return Timestamp{std::time(nullptr)};
    }
}; // namespace utils::time
