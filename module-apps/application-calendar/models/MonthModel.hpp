#pragma once

#include "DayModel.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <gui/widgets/Item.hpp>
#include <Text.hpp>
#include <map>
#include <string>
#include <vector>

class MonthModel
{
  public:
    std::string name;
    std::vector<DayModel> days;
    uint32_t numberOfDays = 0;

  public:
    MonthModel(std::string name, std::vector<DayModel> days);
    MonthModel()          = default;
    virtual ~MonthModel() = default;
};
