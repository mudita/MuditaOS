// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <time/dateCommon.hpp>

#include <string>
#include <vector>

class YearMonth
{
    date::year year;
    date::month month;

  public:
    YearMonth() = default;
    explicit YearMonth(date::year_month_day ymd) : year{ymd.year()}, month{ymd.month()} {};

    bool operator>(const YearMonth &other) const noexcept
    {
        if (this->year != other.year) {
            return this->year > other.year;
        }
        else {
            return this->month > other.month;
        }
    };

    bool operator==(const YearMonth &other) const noexcept
    {
        return this->month == other.month && this->year == other.year;
    };

    bool operator!=(const YearMonth &other)
    {
        return !(*this == other);
    };

    bool operator<(const YearMonth &other)
    {
        return !(*this > other) && (*this != other);
    };
};

class MonthModel
{
  private:
    YearMonth yearMonth;

  public:
    explicit MonthModel(date::year_month_day yearMonthDay);
    MonthModel()          = default;
    virtual ~MonthModel() = default;

    date::year getYear();
    date::month getMonth();
    uint32_t getLastDay();
    uint32_t getFirstWeekOffset();
    std::string getMonthYearText();
    std::string getMonthText();
    std::vector<std::string> split(const std::string &s, char delim);

    date::month month;
    uint32_t lastDay;
    // first week offset
    uint32_t firstWeekDayNumber;
    date::year year;
};
