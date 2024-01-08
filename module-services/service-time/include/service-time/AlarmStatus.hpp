// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace alarms
{
    enum class AlarmStatus
    {
        Invalid,
        Deactivated,
        Activated,
        Snoozed,
        Ringing,
        Queued
    };
} // namespace alarms
