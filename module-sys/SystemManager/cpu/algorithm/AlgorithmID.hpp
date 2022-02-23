// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once 

namespace sys::cpu
{
    enum class AlgoID
    {
        None,
        FrequencyHold,
        ImmediateUpscale,
        FrequencyStepping,
    };
}
