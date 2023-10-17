// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-calendar/widgets/CalendarStyle.hpp>

#include <module-gui/gui/SwitchData.hpp>
#include <time/dateCommon.hpp>

enum class EventAction
{
    Add,
    Edit
};

class WeekDaysRepeatData : public gui::SwitchData
{
  protected:
    bool weekDays[style::window::calendar::week_days_number];

  public:
    WeekDaysRepeatData();
    virtual ~WeekDaysRepeatData() = default;
    [[nodiscard]] auto getData(const uint32_t weekDay) const -> bool;
    virtual void setData(const uint32_t weekDay, const bool value);
    virtual void setData(const uint32_t weekDay);
};

class DayMonthData : public gui::SwitchData
{
  protected:
    std::string dayMonth;
    TimePoint dateFilter;

  public:
    DayMonthData()          = default;
    virtual ~DayMonthData() = default;
    std::string getDayMonthText()
    {
        return dayMonth;
    };

    TimePoint getDateFilter()
    {
        return dateFilter;
    };

    virtual void setData(const std::string &dayMonthText, const TimePoint &dateNumb)
    {
        dayMonth   = dayMonthText;
        dateFilter = dateNumb;
    };
};
