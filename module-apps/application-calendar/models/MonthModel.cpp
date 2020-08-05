#include "MonthModel.hpp"

MonthModel::MonthModel(year_month_day ymd)
{
    this->m                  = ymd.month();
    this->y                  = ymd.year();
    year_month_day_last ymdl = this->y / this->m / last;
    this->lastDay            = unsigned{ymdl.day()};
    year_month_day ymdf      = this->y / this->m / 1;
    this->offset             = weekday{ymdf}.c_encoding();
}

month MonthModel::getMonth()
{
    return this->m;
}

uint32_t MonthModel::getLastDay()
{
    return this->lastDay;
}

uint32_t MonthModel::getFirstWeekOffset()
{
    if (this->offset == 0) {
        return 7;
    }
    else {
        return this->offset;
    }
}

std::string MonthModel::getMonthYearText()
{
    int yearUInt        = static_cast<decltype(yearUInt)>(y);
    std::string yearStr = std::to_string(yearUInt);
    std::string monthStr;
    unsigned int monthUInt = unsigned{m};

    switch (monthUInt) {
    case 1:
        monthStr = "January";
        break;
    case 2:
        monthStr = "February";
        break;
    case 3:
        monthStr = "March";
        break;
    case 4:
        monthStr = "April";
        break;
    case 5:
        monthStr = "May";
        break;
    case 6:
        monthStr = "June";
        break;
    case 7:
        monthStr = "July";
        break;
    case 8:
        monthStr = "August";
        break;
    case 9:
        monthStr = "September";
        break;
    case 10:
        monthStr = "October";
        break;
    case 11:
        monthStr = "November";
        break;
    case 12:
        monthStr = "December";
        break;
    default:
        monthStr = "";
    }

    return monthStr + " " + yearStr;
}
