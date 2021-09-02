// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-time/AlarmHandler.hpp>

namespace alarms
{

    class BellAlarmClockHandler : public AlarmHandler
    {
      public:
        auto handle(const AlarmEventRecord &record) -> bool;
    };

    class EveningReminderHandler : public AlarmHandler
    {
      public:
        auto handle(const AlarmEventRecord &record) -> bool;
    };
} // namespace alarms
