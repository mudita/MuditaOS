// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace cellular
{
    struct VolteState
    {
        enum class Enablement
        {
            On,
            Off,
            SwitchingToOff,
            SwitchingToOn,
            Undefined
        } enablement   = Enablement::Undefined;
        bool permitted = false;
    };
}
