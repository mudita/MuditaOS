// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace alarms
{
    class AbstractAlarmAction
    {
      public:
        virtual ~AbstractAlarmAction() = default;
        virtual bool execute()         = 0;
        virtual bool turnOff()         = 0;
    };
} // namespace alarms
