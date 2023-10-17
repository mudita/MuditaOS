// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/AlarmEventsTable.hpp>

namespace alarms
{
    class AbstractAlarmAction
    {
      public:
        virtual ~AbstractAlarmAction()                       = default;
        virtual bool execute(const AlarmEventRecord &record) = 0;
        virtual bool turnOff(const AlarmEventRecord &record) = 0;
    };
} // namespace alarms
