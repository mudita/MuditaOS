// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-db/Interface/AlarmsRecord.hpp>
#include <SwitchData.hpp>

enum class AlarmRepeat
{
    never,
    everyday,
    weekDays
};

class AlarmRecordData : public gui::SwitchData
{
  protected:
    std::shared_ptr<AlarmsRecord> record;

  public:
    explicit AlarmRecordData(std::shared_ptr<AlarmsRecord> record) : record{std::move(record)}
    {}
    std::shared_ptr<AlarmsRecord> getData() const
    {
        return record;
    }
    void setData(std::shared_ptr<AlarmsRecord> rec)
    {
        record = std::move(rec);
    }
};
