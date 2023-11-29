// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app::relaxation
{
    constexpr auto timerValueDBRecordName = "RelaxationTimerValue";

    enum class MusicType
    {
        Relaxation,
        ColorsOfNoise,
        User,
        Undefined
    };
} // namespace app::relaxation
