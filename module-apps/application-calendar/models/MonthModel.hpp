#pragma once

#include "DayModel.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <gui/widgets/Item.hpp>
#include <Text.hpp>
#include <application-calendar/date.h>
#include <map>
#include <string>
#include <vector>

using namespace date;

class MonthModel
{
  public:
    month m;
    day lastDay;
    // first week offset
    uint32_t offset;
    year y;

  public:
    MonthModel(year_month_day ymd);
    MonthModel()          = default;
    virtual ~MonthModel() = default;

    year getYear();
    month getMonth();
    day getLastDay();
    uint32_t getFirstWeekOffset();
};
