#pragma once
#include "module-db/Interface/EventsRecord.hpp"
#include <module-gui/gui/SwitchData.hpp>
#include "application-calendar/widgets/CalendarStyle.hpp"

class EventRecordData : public gui::SwitchData
{
  protected:
    std::string backWindowName;
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
    std::string getWindowName()
    {
        return backWindowName;
    };
    virtual void setWindowName(const std::string &windowName)
    {
        backWindowName = windowName;
    };
};

class WeekDaysRepeatData : public gui::SwitchData
{
  protected:
    bool weekDays[style::window::calendar::week_days_number];

  public:
    WeekDaysRepeatData();
    virtual ~WeekDaysRepeatData() = default;
    [[nodiscard]] auto getData(const uint32_t &weekDay) const -> bool;
    virtual void setData(const uint32_t &weekDay);
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

    virtual void setData(std::string dayMonthText, const TimePoint &dateNumb)
    {
        dayMonth   = dayMonthText;
        dateFilter = dateNumb;
    };
};

class PrevWindowData : public gui::SwitchData
{
  public:
    enum PrevWindow
    {
        Delete = 0,
        Edit,
        Add
    };

  protected:
    PrevWindow prevWindow;

  public:
    PrevWindowData()          = default;
    virtual ~PrevWindowData() = default;

    PrevWindow getData()
    {
        return prevWindow;
    };

    virtual void setData(PrevWindow data)
    {
        prevWindow = data;
    };
};
