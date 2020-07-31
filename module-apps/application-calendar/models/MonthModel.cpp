#include "MonthModel.hpp"

MonthModel::MonthModel(year_month_day ymd)
{
    this->m                  = ymd.month();
    this->offset             = weekday{ymd}.c_encoding();
    this->y                  = ymd.year();
    year_month_day_last ymdl = this->y / this->m / last;
    this->lastDay            = ymdl.day();
}

month MonthModel::getMonth()
{
    return this->m;
}

day MonthModel::getLastDay()
{
    return this->lastDay;
}

uint32_t MonthModel::getFirstWeekOffset()
{
    return this->offset;
}
