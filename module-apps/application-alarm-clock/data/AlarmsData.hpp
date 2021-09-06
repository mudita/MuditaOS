// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/AlarmEventRecord.hpp>
#include <SwitchData.hpp>

enum class AlarmAction
{
    Add,
    Edit
};

class AlarmRecordData : public gui::SwitchData
{
  protected:
    std::shared_ptr<AlarmEventRecord> record;

  public:
    explicit AlarmRecordData(std::shared_ptr<AlarmEventRecord> record) : record{std::move(record)}
    {}
    std::shared_ptr<AlarmEventRecord> getData()
    {
        return record;
    }
    void setData(std::shared_ptr<AlarmEventRecord> rec)
    {
        record = std::move(rec);
    }
};

