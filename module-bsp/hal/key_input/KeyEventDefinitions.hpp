// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <magic_enum.hpp>

namespace bsp
{

    enum class KeyCodes
    {
        Undefined     = 0,
        NumericKey1   = 31,
        NumericKey2   = 32,
        NumericKey3   = 33,
        NumericKey4   = 41,
        NumericKey5   = 42,
        NumericKey6   = 43,
        NumericKey7   = 51,
        NumericKey8   = 52,
        NumericKey9   = 53,
        NumericKey0   = 62,
        NumericKeyAst = 61,
        NumericKeyPnd = 63,

        JoystickLeft  = 11,
        JoystickRight = 13,
        JoystickUp    = 2,
        JoystickDown  = 22,
        JoystickEnter = 12,

        FnLeft  = 21, // 1,
        FnRight = 23, // 3,

        VolUp   = 4,
        VolDown = 14,

        Torch = 24,

        SSwitchUp   = 34,
        SSwitchDown = 54,
        SSwitchMid  = 44,

        HeadsetOk      = 71,
        HeadsetVolUp   = 72,
        HeadsetVolDown = 73
    };

    enum class KeyEvents
    {
        Released,
        Pressed,
        Moved,
    };

    struct KeyEvent
    {
        KeyCodes code;
        KeyEvents event;
    };
} // namespace bsp

inline const char *c_str(bsp::KeyCodes code)
{
    return magic_enum::enum_name(code).data();
}
