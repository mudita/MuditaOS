// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/AlarmEventRecord.hpp>

struct SnoozedAlarmEventRecord : public SingleEventRecord
{
  public:
    std::uint32_t snoozeCount = 1;

    explicit SnoozedAlarmEventRecord(SingleEventRecord *singleAlarm)
        : SingleEventRecord(singleAlarm->parent, singleAlarm->startDate, singleAlarm->endDate){};

    std::uint32_t snooze() noexcept
    {
        return ++snoozeCount;
    }
};
