// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/Record.hpp>
#include <cstdint>

namespace alarms
{
    class AbstractAlarmAction
    {
      public:
        virtual ~AbstractAlarmAction()      = default;
        virtual bool execute(Record record) = 0;
        virtual bool turnOff()              = 0;
    };
} // namespace alarms
