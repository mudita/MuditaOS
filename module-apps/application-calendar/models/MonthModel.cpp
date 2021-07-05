// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MonthModel.hpp"
#include <time/time_locale.hpp>
#include <time/time_constants.hpp>

date::year_month_day MonthModel::getYearMonthDayFromTimePoint(TimePoint timePoint) const
{
    return date::year_month_day{date::floor<date::days>(timePoint)};
}

uint32_t MonthModel::getEventDurationInDays(const EventsRecord &record) const
{
    auto eventStartDuration = utils::time::Duration(TimePointToTimeT(record.date_from));
    auto eventEndDuration   = utils::time::Duration(TimePointToTimeT(record.date_till));
    auto startEventDurationSinceEpochInDaysRoundedDown =
        (eventStartDuration.getHours() - eventStartDuration.getHours() % utils::time::hoursInday) /
        utils::time::hoursInday;
    auto endEventDurationSinceEpochInDaysRoundedDown =
        (eventEndDuration.getHours() - eventEndDuration.getHours() % utils::time::hoursInday) / utils::time::hoursInday;
    return endEventDurationSinceEpochInDaysRoundedDown - startEventDurationSinceEpochInDaysRoundedDown;
}

uint32_t MonthModel::getDayIndex(TimePoint date) const
{
    date::year_month_day recordDate = TimePointToYearMonthDay(date);
    return (static_cast<unsigned>(recordDate.day()) - 1);
}

MonthModel::MonthModel(date::year_month_day yearMonthDay)
{
    this->yearMonth                            = YearMonth(yearMonthDay);
    this->month                                = yearMonthDay.month();
    this->year                                 = yearMonthDay.year();
    date::year_month_day_last yearMonthDayLast = this->year / this->month / date::last;
    this->lastDay                              = static_cast<unsigned>(yearMonthDayLast.day());
    date::year_month_day yearMonthDayFirst     = this->year / this->month / 1;
    this->firstWeekDayNumb                     = date::weekday{yearMonthDayFirst}.c_encoding();
}

void MonthModel::markEventsInDays(const std::vector<EventsRecord> &records, std::array<bool, 31> &isDayEmpty)
{
    for (auto &rec : records) {
        auto eventYearMonthFrom = YearMonth(getYearMonthDayFromTimePoint(rec.date_from));
        auto eventYearMonthTill = YearMonth(getYearMonthDayFromTimePoint(rec.date_till));

        if (eventYearMonthFrom < this->yearMonth && this->yearMonth < eventYearMonthTill) {
            for (uint32_t i = 0; i < max_month_day; i++) {
                isDayEmpty[i] = false;
            }
            return;
        }

        int durationInDays = getEventDurationInDays(rec);

        if (this->yearMonth == eventYearMonthFrom) {
            for (int i = getDayIndex(rec.date_from); i < max_month_day && durationInDays >= 0; i++, durationInDays--) {
                isDayEmpty[i] = false;
            }
        }

        if (this->yearMonth == eventYearMonthTill) {
            for (int i = getDayIndex(rec.date_till); i >= 0 && durationInDays >= 0; i--, durationInDays--) {
                isDayEmpty[i] = false;
            }
        }
    }
}

date::year MonthModel::getYear()
{
    return year;
}

date::month MonthModel::getMonth()
{
    return month;
}

uint32_t MonthModel::getLastDay()
{
    return this->lastDay;
}

uint32_t MonthModel::getFirstWeekOffset()
{
    if (this->firstWeekDayNumb == 0) {
        return 6;
    }
    else {
        return this->firstWeekDayNumb - 1;
    }
}

std::vector<std::string> MonthModel::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::string MonthModel::getMonthText()
{
    auto monthUInt       = static_cast<unsigned>(month);
    std::string monthStr = utils::time::Locale::get_month(utils::time::Locale::Month(monthUInt - 1));
    return monthStr;
}

std::string MonthModel::getMonthYearText()
{
    auto yearInt         = static_cast<int>(year);
    std::string yearStr  = std::to_string(yearInt);
    auto monthUInt       = static_cast<unsigned>(month);
    std::string monthStr = utils::time::Locale::get_month(utils::time::Locale::Month(monthUInt - 1));

    return monthStr + " " + yearStr;
}
