// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/AlarmEventRecord.hpp>

namespace alarms
{
    enum class AlarmType
    {
        Clock,
        Calendar,
        EveningReminder,
        None
    };

    class AlarmHandler
    {
      public:
        virtual ~AlarmHandler() = default;

        virtual auto handle(const AlarmEventRecord &record) -> bool = 0;
    };
} // namespace alarms
