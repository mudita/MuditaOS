// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-calendar/data/OptionParser.hpp"
#include <module-db/Interface/AlarmsRecord.hpp>
#include <SwitchData.hpp>

enum class AlarmRepeat
{
    never,
    everyday,
    weekDays
};

enum class AlarmSnooze
{
    FiveMinutes    = 5,
    TenMinutes     = 10,
    FifteenMinutes = 15,
    ThirtyMinutes  = 30
};

enum class AlarmOptionItemName
{
    Sound,
    Snooze,
    Repeat
};

enum class AlarmAction
{
    Add,
    Edit
};

enum class WeekDayIso
{
    Monday = date::Monday.iso_encoding() - 1,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

class AlarmRecordData : public gui::SwitchData
{
  protected:
    std::shared_ptr<AlarmsRecord> record;

  public:
    explicit AlarmRecordData(std::shared_ptr<AlarmsRecord> record) : record{std::move(record)}
    {}
    std::shared_ptr<AlarmsRecord> getData()
    {
        return record;
    }
    void setData(std::shared_ptr<AlarmsRecord> rec)
    {
        record = std::move(rec);
    }
};

class AlarmRecordsData : public gui::SwitchData
{
    std::vector<AlarmsRecord> records;

  public:
    explicit AlarmRecordsData(std::vector<AlarmsRecord> records) : records{std::move(records)}
    {}
    std::vector<AlarmsRecord> getRecords()
    {
        return records;
    }
};

class CustomRepeatValueParser
{
    std::unique_ptr<WeekDaysRepeatData> weekDayData;

  public:
    explicit CustomRepeatValueParser(uint32_t repeatValue);

    [[nodiscard]] std::string getWeekDaysText() const;
    [[nodiscard]] bool isCustomValueWeekDays() const;
    [[nodiscard]] bool isCustomValueEveryday() const;
};
