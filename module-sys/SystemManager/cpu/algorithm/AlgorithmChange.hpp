// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys::cpu::algorithm
{
    enum class Change
    {
        UpScaled,   /// frequency risen
        Downscaled, /// frequency downscaled
        Hold,       /// frequency requested to be held
        NoChange    /// nothing to do
    };
}
