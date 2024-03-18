// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bsp
{
    enum class NotificationSource : std::uint8_t
    {
        LeftSideKeyPress = 1,
        LeftSideKeyRelease,
        RightSideKeyPress,
        RightSideKeyRelease,
        LightCenterKeyPress,
        LightCenterKeyRelease,
        LatchKeyPress,
        LatchKeyRelease,
        RotaryEncoder,
        Invalid = 0xFF
    };
} // namespace bsp
