// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MonthModel.hpp"

#include <time/time_constants.hpp>
#include <time/time_locale.hpp>

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
