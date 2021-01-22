// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "module-db/Interface/EventsRecord.hpp"
#include <module-gui/gui/SwitchData.hpp>
#include "application-calendar/widgets/CalendarStyle.hpp"

class EventRecordData : public gui::SwitchData
{
  protected:
    std::shared_ptr<EventsRecord> record;

  public:
    EventRecordData() = default;
    EventRecordData(std::shared_ptr<EventsRecord> record) : record{std::move(record)} {};
    virtual ~EventRecordData() = default;
    std::shared_ptr<EventsRecord> getData()
    {
        return record;
    };
    virtual void setData(std::shared_ptr<EventsRecord> rec)
    {
        record = std::move(rec);
    };
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
    calendar::TimePoint dateFilter;

  public:
    DayMonthData()          = default;
    virtual ~DayMonthData() = default;
    std::string getDayMonthText()
    {
        return dayMonth;
    };

    calendar::TimePoint getDateFilter()
    {
        return dateFilter;
    };

    virtual void setData(std::string dayMonthText, const calendar::TimePoint &dateNumb)
    {
        dayMonth   = dayMonthText;
        dateFilter = dateNumb;
    };
};
