// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

namespace utils::time
{
    using namespace std::chrono_literals;

    namespace duration
    {
        static constexpr auto durationFormatH0M0S = "duration_hour_0min_0sec";
        static constexpr auto durationFormat0M0S  = "duration_0min_0sec";
        static constexpr auto durationFormat0N0S  = "duration_0hmin_0sec";
        static constexpr auto durationFormatM0S   = "duration_min_0sec";

        constexpr auto abbrev_len = 3U;

        /// order matters, it's used in replace_locale with enum Replacements
        const std::vector<std::string> specifiers_replacement = {"%a",  // day abbrew
                                                                 "%A",  // day long
                                                                 "%b",  // month abbrew
                                                                 "%B",  // month long
                                                                 "%Z"}; // timezone

        struct Format
        {
            const std::string lowFormat;  /// format used if duration is below 1 hour
            const std::string highFormat; /// format used if duration exceeds 1 hour
        };

        using FormatMap                         = std::map<const Duration::DisplayedFormat, const Format>;
        static const inline FormatMap formatMap = {
                {Duration::DisplayedFormat::Fixed0M0S, {durationFormat0N0S, durationFormat0N0S}},
                {Duration::DisplayedFormat::FixedH0M0S, {durationFormatH0M0S, durationFormatH0M0S}},
                {Duration::DisplayedFormat::AutoM, {durationFormatM0S, durationFormatH0M0S}},
                {Duration::DisplayedFormat::Auto0M, {durationFormat0M0S, durationFormatH0M0S}}
        };
    }


    Locale tlocale;
    static int msTimeGmtOff = 4 * utils::time::minutes_in_quarter_hour * std::chrono::seconds(1min).count();

    UTF8 Localer::get_replacement(Replacements val, const struct tm &timeinfo)
    {
        UTF8 retval = "";
        switch (val) {
            case Replacements::DayLong:
                retval = Locale::get_day(Locale::Day(timeinfo.tm_wday));
                break;
            case Replacements::DayAbbrev:
                retval = Locale::get_day(Locale::Day(timeinfo.tm_wday)).substr(0, duration::abbrev_len);
                break;
            case Replacements::MonthLong:
                retval = Locale::get_month(date::month(timeinfo.tm_mon+1));
                break;
            case Replacements::MonthAbbrev:
                retval = Locale::get_month(date::month(timeinfo.tm_mon+1)).substr(0, duration::abbrev_len);
                break;
            case Replacements::Timezone:
                break;
        }
        return retval;
    }

    std::tm TimeConversion::toTm(const std::time_t &t)
    {
        return *gmtime(&t);
    }

    std::tm TimeConversion::toTm(const utils::time::TimePoint &tp)
    {
        return toTm(toTime_t(tp));
    }

    utils::time::TimePoint TimeConversion::toTimePoint(const std::time_t &t)
    {
        return std::chrono::system_clock::from_time_t(t);
    }

    utils::time::TimePoint TimeConversion::toTimePoint(const std::tm &tm)
    {
        return std::chrono::system_clock::from_time_t(toTime_t(tm));
    }

    time_t TimeConversion::toTime_t(const std::tm &tm)
    {
        std::tm info = tm;
        return std::mktime(&info);
    }

    time_t TimeConversion::toTime_t(const utils::time::TimePoint &tp)
    {
        return std::chrono::system_clock::to_time_t(tp);
    }

    std::chrono::seconds SysTimeBase::getLocalGMTTimeOffsetFromGMT(const utils::time::TimePoint &gmtTp) const
    {
        const time_t &gmtTime = TimeConversion::toTime_t(gmtTp);

        std::tm tmLocal  = *std::localtime(&gmtTime);
        tmLocal.tm_isdst = -1;

        std::tm tmGMT  = *std::gmtime(&gmtTime);
        tmGMT.tm_isdst = -1;

        return std::chrono::duration_cast<std::chrono::seconds>(TimeConversion::toTimePoint(tmLocal) - TimeConversion::toTimePoint(tmGMT));
    }

    std::chrono::seconds SysTimeBase::getLocalGMTTimeOffsetFromLocal(const utils::time::TimePoint &localTp) const
    {
        const time_t &localTime = TimeConversion::toTime_t(localTp);

        std::tm tm  = *std::gmtime(&localTime);
        tm.tm_isdst = -1;

        return std::chrono::duration_cast<std::chrono::seconds>(localTp - TimeConversion::toTimePoint(tm));
    }


    std::pair<TimePoint, std::chrono::seconds> SysTimeBase::getCurrentTime() const
    {
        time_t t;
        auto err = bsp::rtc_GetCurrentTimestamp(&t);
        if (err) {
        LOG_ERROR("rtc_GetCurrentDateTimestamp failure!");
        }

        utils::time::TimePoint gmtTime   = TimeConversion::toTimePoint(t);
        utils::time::TimePoint localTime = localTimeFromGMTTime(gmtTime);

        return std::make_pair(gmtTime, std::chrono::duration_cast<std::chrono::seconds>(localTime - gmtTime));
    }

    utils::time::TimePoint SysTimeBase::localTimeFromGMTTime(const utils::time::TimePoint &gmtTp) const
    {
        return gmtTp + getLocalGMTTimeOffsetFromGMT(gmtTp);
    }

    utils::time::TimePoint SysTimeBase::gmtTimeFromLocalTime(const utils::time::TimePoint &localTp) const
    {
        return localTp - getLocalGMTTimeOffsetFromLocal(localTp);
    }


    SysTimeBase &Timestamp::getSysTimeBase()
    {
        return sysTimeBase;
    };

    utils::time::TimePoint Timestamp::getGMTTime() const
    {
        return gmtTime;
    }

    utils::time::TimePoint Timestamp::getLocalTime() const
    {
        return localTime;
    }

    std::chrono::seconds Timestamp::getOffset() const
    {
        return std::chrono::duration_cast<std::chrono::seconds>(localTime - gmtTime);
    }

    Timestamp::operator UTF8 *() const
    {
        return nullptr;
    }

    Timestamp::operator TimePoint()
    {
        return getLocalTime();
    }

    Timestamp::Timestamp()
    {
        setTime();
    }

    Timestamp::Timestamp(const utils::time::TimePoint &newGMT)
    {
        setTime(newGMT);
    }

    Timestamp::Timestamp(const utils::time::TimePoint &newGMT, std::chrono::seconds offset)
    {
        setTime(newGMT, offset);
    }

    Timestamp::Timestamp(const utils::time::TimePoint &newTime, TimestampType type)
    {
        setTime(newTime, type);
    }

    Timestamp::Timestamp(const utils::time::TimePoint &newTime, TimestampType type, std::chrono::seconds offset)
    {
        setTime(newTime, type, offset);
    }

    Timestamp::Timestamp(const utils::time::TimePoint &newGMT, const utils::time::TimePoint &newLocal)
    {
        setTime(newGMT, newLocal);
    }

    void Timestamp::setTime()
    {
        auto [gmt, off] = sysTimeBase.getCurrentTime();
        gmtTime         = gmt;
        localTime       = gmt + off;
    }

    void Timestamp::setTime(const utils::time::TimePoint &newGMT)
    {
        gmtTime   = newGMT;
        localTime = sysTimeBase.localTimeFromGMTTime(gmtTime);
    }

    void Timestamp::setTime(const utils::time::TimePoint &newGMT, std::chrono::seconds offset)
    {
        gmtTime   = newGMT;
        localTime = gmtTime + offset;
    }

    void Timestamp::setTime(const utils::time::TimePoint &newTime, TimestampType type)
    {
        switch (type)
        {
            case TimestampType::useAsLocaltime:
            {
                gmtTime = sysTimeBase.gmtTimeFromLocalTime(newTime);
                localTime = newTime;
                break;
            }
            case TimestampType::useAsGMT:
            {
                setTime(newTime);
                break;
            }
        }
    }

    void Timestamp::setTime(const utils::time::TimePoint &newTime, TimestampType type, std::chrono::seconds offset)
    {
        switch (type)
        {
            case TimestampType::useAsLocaltime:
            {
                gmtTime   = newTime - offset;
                localTime = newTime;
                break;
            }
            case TimestampType::useAsGMT:
            {
                setTime(newTime, offset);
                break;
            }
        }
    }

    void Timestamp::setTime(const utils::time::TimePoint &newGMT, const utils::time::TimePoint &newLocal)
    {
        gmtTime   = newGMT;
        localTime = newLocal;
    }

    void Timestamp::set_format(std::string format)
    {
        this->format = format;
    }

    Timestamp::operator YearMonthDay()
    {
        return CalendarConversion::TimePointToYearMonthDay(getLocalTime());
    }

    Timestamp::operator HourMinuteSecond()
    {
        return CalendarConversion::TimePointToHourMinSec(getLocalTime());
    }

    Timestamp::operator date::year()
    {
        return CalendarConversion::TimePointToYearMonthDay(getLocalTime()).year();
    }
    Timestamp::operator date::month()
    {
        return CalendarConversion::TimePointToYearMonthDay(getLocalTime()).month();
    }
    Timestamp::operator date::day()
    {
        return CalendarConversion::TimePointToYearMonthDay(getLocalTime()).day();
    }
    Timestamp::operator std::chrono::hours()
    {
        return CalendarConversion::TimePointToHourMinSec(getLocalTime()).hours();
    }
    Timestamp::operator std::chrono::minutes()
    {
        return CalendarConversion::TimePointToHourMinSec(getLocalTime()).minutes();
    }
    Timestamp::operator std::chrono::seconds()
    {
        return CalendarConversion::TimePointToHourMinSec(getLocalTime()).seconds();
    }

    Timestamp &Timestamp::operator-=(const std::chrono::seconds &rhs)
    {
        this->setTime(this->getLocalTime() - rhs, TimestampType::useAsLocaltime);
        return *this;
    }

    Timestamp &Timestamp::operator+=(const std::chrono::seconds &rhs)
    {
        this->setTime(this->getLocalTime() + rhs, TimestampType::useAsLocaltime);
        return *this;
    }

    constexpr uint32_t datasize = 128;
    UTF8 Timestamp::str(std::string fmt)
    {
        if (fmt.compare("") != 0) {
            this->format = fmt;
        }
        UTF8 datetimestr = "";
        auto timeinfo    = TimeConversion::toTm(getLocalTime());
        auto replaceFunc = [&](int idx) { return get_replacement(Replacements(idx), timeinfo); };
        utils::findAndReplaceAll(this->format, duration::specifiers_replacement, replaceFunc);
        auto data = std::unique_ptr<char[]>(new char[datasize]);
        std::strftime(data.get(), datasize, this->format.c_str(), &timeinfo);
        datetimestr = UTF8(data.get());
        return datetimestr;
    }

    UTF8 Timestamp::day(bool abbrev)
    {
        if (abbrev) {
            return get_replacement(Replacements::DayAbbrev, TimeConversion::toTm(getLocalTime()));
        }
        else {
            return get_replacement(Replacements::DayLong, TimeConversion::toTm(getLocalTime()));
        }
    }

    UTF8 Timestamp::month(bool abbrev)
    {
        if (abbrev) {
            return get_replacement(Replacements::MonthAbbrev, TimeConversion::toTm(getLocalTime()));
        }
        else {
            return get_replacement(Replacements::MonthLong, TimeConversion::toTm(getLocalTime()));
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
            return ((std::chrono::hours) * this).count();
        case GetParameters::Minute:
            return ((std::chrono::minutes) * this).count();
        case GetParameters::Second:
            return ((std::chrono::seconds) * this).count();
        case GetParameters::Day:
            return static_cast<unsigned>((date::day)(*this));
        case GetParameters::Month:
            return static_cast<unsigned>((date::month)(*this));
        case GetParameters::Year:
            return static_cast<int>((date::year)(*this));
        }
        return UINT32_MAX;
    }

    DateTime::DateTime(bool date_format_long) : date_format_long(date_format_long)
    {
        setTime();
    }

    DateTime::DateTime(const utils::time::TimePoint &tp, bool date_format_long) : date_format_long(date_format_long)
    {
        setTime(tp);
    }

    std::chrono::seconds Duration::get() const
    {
        return duration;
    }

    std::chrono::minutes Duration::getMinutes() const
    {
        return _minutes;
    }

    std::chrono::hours Duration::getHours() const
    {
        return _hours;
    }

    date::days Duration::getRoundedDays() const
    {
        return _roundedDays;
    }

    Duration::Duration(const utils::time::TimePoint &stop, const utils::time::TimePoint &start)
        : Duration(stop - start > std::chrono::seconds(0) ? std::chrono::duration_cast<std::chrono::seconds>(stop - start) : std::chrono::seconds(0))
    {
        duration > std::chrono::seconds(0) ? _roundedDays = date::days{date::floor<date::days>(stop) - date::floor<date::days>(start)} : date::days{0};
    }

    Duration::Duration(const Timestamp &stop, const Timestamp &start) : Duration(stop.getLocalTime(), start.getLocalTime())
    {}

    void Duration::fillStr(std::string &format) const
    {
        constexpr auto numberOfLeadingDigits = 2;
        utils::findAndReplaceAll(format, "%H", utils::to_string(_hours.count()));
        utils::findAndReplaceAll(format, "%M", utils::to_string(_minutes.count()));
        utils::findAndReplaceAll(format, "%N", utils::to_string(_hmminutes.count()));
        utils::findAndReplaceAll(format, "%S", utils::to_string(_seconds.count()));
        utils::findAndReplaceAll(
            format, "%0H", utils::addLeadingZeros(utils::to_string(_hours.count()), numberOfLeadingDigits));
        utils::findAndReplaceAll(
            format, "%0M", utils::addLeadingZeros(utils::to_string(_minutes.count()), numberOfLeadingDigits));
        utils::findAndReplaceAll(
            format, "%0N", utils::addLeadingZeros(utils::to_string(_hmminutes.count()), numberOfLeadingDigits));
        utils::findAndReplaceAll(
            format, "%0S", utils::addLeadingZeros(utils::to_string(_seconds.count()), numberOfLeadingDigits));
    }

    UTF8 Duration::str(DisplayedFormat displayedFormat) const
    {
        // switch between format low and hig
        std::string data = utils::localize.get(_hours != std::chrono::hours(0) ? duration::formatMap.at(displayedFormat).highFormat
                                                                  : duration::formatMap.at(displayedFormat).lowFormat);
        fillStr(data);

        return data;
    }

    Duration operator-(const Timestamp &lhs, const Timestamp &rhs)
    {
        return Duration(lhs.getLocalTime(), rhs.getLocalTime());
    };

    Timestamp operator-(const Timestamp &lhs, const std::chrono::seconds &rhs)
    {
        return Timestamp(lhs.getLocalTime() - rhs, TimestampType::useAsLocaltime);
    };
    Timestamp operator+(const Timestamp &lhs, const std::chrono::seconds &rhs)
    {
        return Timestamp(lhs.getLocalTime() + rhs, TimestampType::useAsLocaltime);
    };
    Timestamp operator+(const std::chrono::seconds &lhs, const Timestamp &rhs)
    {
        return Timestamp(rhs.getLocalTime() + lhs, TimestampType::useAsLocaltime);
    }

    bool DateTime::isToday()
    {
        auto [gmt, off] = getSysTimeBase().getCurrentTime();
        auto todayLocal = gmt + off;
        auto todayYMD   = CalendarConversion::TimePointToYearMonthDay(todayLocal);
        auto storedYMD  = CalendarConversion::TimePointToYearMonthDay(this->getLocalTime());

        return (storedYMD == todayYMD);
    }

    bool DateTime::isYesterday()
    {
        auto [gmt, off]     = getSysTimeBase().getCurrentTime();
        auto yesterdayLocal = gmt + off - date::days(1);
        auto yesterdayYMD   = CalendarConversion::TimePointToYearMonthDay(yesterdayLocal);
        auto storedYMD      = CalendarConversion::TimePointToYearMonthDay(this->getLocalTime());

        return (storedYMD == yesterdayYMD);
    }

    UTF8 DateTime::str(std::string format)
    {
        if (format.compare("") != 0) {
            return Timestamp::str(format);
        }
        if (isToday()) // if the same computer day, then return hour.
        {
            return Timestamp::str(Locale::format(time_format_24h ? Locale::FormatTime24H : Locale::FormatTime12H));
        }
        else if (show_textual_past && isYesterday()) {
            return Locale::yesterday();
        }
        else {
            std::string fmt =
                Locale::format(date_format_long ? Locale::FormatLocaleDateFull : Locale::FormatLocaleDateShort) + " " +
                Locale::format(time_format_24h ? Locale::FormatTime24H : Locale::FormatTime12H);
            return Timestamp::str(fmt);
        }
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
            return Timestamp::str(Locale::format(time_format_24h ? Locale::FormatTime24H : Locale::FormatTime12H));
        }
    }

    void Time::setTimeZoneOffset(int tzOffset)
    {
        msTimeGmtOff = tzOffset;
    }

    int Time::getTimeZoneOffset()
    {
        return msTimeGmtOff;
    };

    std::string CalendarConversion::TimePointToString(const utils::time::TimePoint &tp)
    {
        return date::format("%F %T", std::chrono::time_point_cast<std::chrono::seconds>(tp));
    }

    utils::time::TimePoint CalendarConversion::TimePointFromString(const char *s1)
    {
        utils::time::TimePoint tp;
        std::istringstream(s1) >> date::parse("%F %T", tp);
        return tp;
    }

    utils::time::HourMinuteSecond CalendarConversion::TimePointToHourMinSec(const utils::time::TimePoint &tp)
    {
        auto dp = date::floor<date::days>(tp);
        return date::make_time(std::chrono::duration_cast<std::chrono::seconds>(tp - dp));
    }

    // 0: Monday, 1: Tuesday ... 6: Sunday
    unsigned int CalendarConversion::WeekdayIndexFromTimePoint(const utils::time::TimePoint &tp)
    {
        auto ymw = date::year_month_weekday{std::chrono::floor<date::days>(tp)};
        return ymw.weekday().iso_encoding() - 1;
    }

    utils::time::TimePoint CalendarConversion::getFirstWeekDay(const utils::time::TimePoint &tp)
    {
        Timestamp ts(tp, TimestampType::useAsLocaltime);
        auto wd = date::weekday{CalendarConversion::TimePointToYearMonthDay(tp)};
        while (wd != date::mon) {
            wd--;
            ts -= date::days(1);
        }
        return ts.getLocalTime();
    }

    std::string CalendarConversion::TimePointToString(const utils::time::TimePoint &tp, date::months months)
    {
        return CalendarConversion::TimePointToString(TimePointAddMonths(tp, months));
    }

    utils::time::TimePoint CalendarConversion::TimePointAddMonths(const utils::time::TimePoint &tp, date::months months)
    {
        date::year_month_day yearMonthDay     = CalendarConversion::TimePointToYearMonthDay(tp);
        auto hourMinuteSecond                 = CalendarConversion::TimePointToHourMinSec(tp);
        date::year_month_day yearMonthDayLast = yearMonthDay.year() / yearMonthDay.month() / date::last;

        int yearShift = (static_cast<unsigned>(yearMonthDay.month()) + months.count()) ? 1 : 0;
        if (yearMonthDayLast.day() == yearMonthDay.day()) {
            yearMonthDayLast =
                    (yearMonthDay.year() + date::years(yearShift)) / (yearMonthDay.month() + months) / date::last;
        }
        else {
            yearMonthDayLast = (yearMonthDay.year() + date::years(yearShift)) / (yearMonthDay.month() + months) /
                               yearMonthDay.day();
        }

        utils::time::TimePoint timePoint =
                date::sys_days{yearMonthDayLast.year() / yearMonthDayLast.month() / yearMonthDayLast.day()};
        timePoint += hourMinuteSecond.to_duration();
        return timePoint;
    }

    std::string CalendarConversion::TimePointToLocalizedDateString(const utils::time::TimePoint &tp, const std::string format)
    {
        utils::time::Date timestamp;
        timestamp.setTime(tp, TimestampType::useAsLocaltime);
        return timestamp.str(format);
    }

    std::string CalendarConversion::TimePointToLocalizedTimeString(const utils::time::TimePoint &tp, const std::string format)
    {
        utils::time::Time timestamp;
        timestamp.setTime(tp, TimestampType::useAsLocaltime);
        return timestamp.str(format);
    }

    utils::time::YearMonthDay CalendarConversion::TimePointToYearMonthDay(const utils::time::TimePoint &tp)
    {
        return date::year_month_day{date::floor<date::days>(tp)};
    }

    utils::time::TimePoint CalendarConversion::TimePointFromYearMonthDay(const utils::time::YearMonthDay &ymd)
    {
        return date::sys_days{ymd.year() / ymd.month() / ymd.day()};
    }

    uint32_t CalendarConversion::TimePointToHour24H(const utils::time::TimePoint &tp)
    {
        return TimePointToHourMinSec(tp).hours().count();
    }

    uint32_t CalendarConversion::TimePointToHour12H(const utils::time::TimePoint &tp)
    {
        auto hms  = TimePointToHourMinSec(tp);
        auto hour = hms.hours().count();
        if (hour > 12) {
            hour -= 12;
        }
        return hour;
    }

    std::string CalendarConversion::TimePointToHourString24H(const utils::time::TimePoint &tp)
    {
        return utils::to_string(TimePointToHour24H(tp));
    }

    std::string CalendarConversion::TimePointToHourString12H(const utils::time::TimePoint &tp)
    {
        return utils::to_string(TimePointToHour12H(tp));
    }

    std::string CalendarConversion::TimePointToMinutesString(const utils::time::TimePoint &tp)
    {
        auto hms          = TimePointToHourMinSec(tp);
        auto minute       = hms.minutes().count();
        auto minuteString = utils::to_string(minute);
        if (minute < 10) {
            minuteString = "0" + minuteString;
        }
        return minuteString;
    }

    std::string CalendarConversion::createUID()
    {
        constexpr uint32_t bufferLimit = 16;
        char Buffer[bufferLimit];
        utils::time::Timestamp timestamp;
        std::string UID{timestamp.str("%Y%m%dT%H%M%S")};
        UID += '-';
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, 100);
        sprintf(Buffer, "%d", distrib(gen));
        UID += Buffer;

        return UID;
    }

    Timestamp getCurrentTimestamp()
    {
        return Timestamp{};
    }

    utils::time::TimePoint TimePointNow()
    {
        auto timePointNow = getCurrentTimestamp();
        return timePointNow.getLocalTime();
    }

}; // namespace utils::time
