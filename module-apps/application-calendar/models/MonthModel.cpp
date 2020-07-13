#include "MonthModel.hpp"

MonthModel::MonthModel(std::string name, std::vector<DayModel> days) : name(std::move(name)), days(days)
{
    LOG_DEBUG("Call month model constructor");
    this->numberOfDays = this->days.size();
}
