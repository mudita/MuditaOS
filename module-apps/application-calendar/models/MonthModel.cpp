#include "MonthModel.hpp"
#include <climits>

MonthModel::MonthModel(const char *name,std::vector <std::unique_ptr<DayModel> > &days)
{
    LOG_DEBUG("Call month model constructor");
    this->name = name;
    swap(this->days,days);
    this->numberOfDays = this->days.size() & INT_MAX;
}