// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-time/AlarmHandler.hpp>

namespace alarms
{

    class MockAlarmHandler : public AlarmHandler
    {
      public:
        auto handle(const AlarmEventRecord &record) -> bool
        {
            LOG_DEBUG("MockAlarmHandler");
            return true;
        }
        auto handleOff(const AlarmEventRecord &record) -> bool
        {
            LOG_DEBUG("MockAlarmHandler");
            return true;
        }
    };
} // namespace alarms
