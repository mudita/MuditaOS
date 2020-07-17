#include "MonthModel.hpp"
#include <climits>

MonthModel::MonthModel(std::string name, std::vector<std::unique_ptr<DayModel>> &days)
{
    LOG_DEBUG("Call month model constructor");
    this->name         = name;
    this->days         = std::move(days);
    this->numberOfDays = this->days.size();
}
