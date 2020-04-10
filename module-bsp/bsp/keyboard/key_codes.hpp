#pragma once

namespace bsp {

    enum class KeyCodes {
        Undefined = 0,
        NumericKey1 = 31,
        NumericKey2 = 32,
        NumericKey3 = 33,
        NumericKey4 = 41,
        NumericKey5 = 42,
        NumericKey6 = 43,
        NumericKey7 = 51,
        NumericKey8 = 52,
        NumericKey9 = 53,
        NumericKey0 = 62,
        NumericKeyAst = 61,
        NumericKeyPnd = 63,

        JoystickLeft = 11,
        JoystickRight = 13,
        JoystickUp = 2,
        JoystickDown = 22,
        JoystickEnter = 12,

        FnLeft = 21,//1,
        FnRight = 23,//3,

        VolUp = 4,
        VolDown = 14,
        Torch = 24,

        SSwitchUp = 34,
        SSwitchDown = 54,
        SSwitchMid = 44
    };
}

inline const char *c_str(bsp::KeyCodes code)
{
    switch (code) {
    case bsp::KeyCodes::Undefined:
        return "Undefined";
    case bsp::KeyCodes::NumericKey1:
        return "NumericKey1";
    case bsp::KeyCodes::NumericKey2:
        return "NumericKey2";
    case bsp::KeyCodes::NumericKey3:
        return "NumericKey3";
    case bsp::KeyCodes::NumericKey4:
        return "NumericKey4";
    case bsp::KeyCodes::NumericKey5:
        return "NumericKey5";
    case bsp::KeyCodes::NumericKey6:
        return "NumericKey6";
    case bsp::KeyCodes::NumericKey7:
        return "NumericKey7";
    case bsp::KeyCodes::NumericKey8:
        return "NumericKey8";
    case bsp::KeyCodes::NumericKey9:
        return "NumericKey9";
    case bsp::KeyCodes::NumericKey0:
        return "NumericKey0";
    case bsp::KeyCodes::NumericKeyAst:
        return "NumericKeyAst";
    case bsp::KeyCodes::NumericKeyPnd:
        return "NumericKeyPnd";
    case bsp::KeyCodes::JoystickLeft:
        return "JoystickLeft";
    case bsp::KeyCodes::JoystickRight:
        return "JoystickRight";
    case bsp::KeyCodes::JoystickUp:
        return "JoystickUp";
    case bsp::KeyCodes::JoystickDown:
        return "JoystickDown";
    case bsp::KeyCodes::JoystickEnter:
        return "JoystickEnter";
    case bsp::KeyCodes::FnLeft:
        return "FnLeft";
    case bsp::KeyCodes::FnRight:
        return "FnRight";
    case bsp::KeyCodes::VolUp:
        return "VolUp";
    case bsp::KeyCodes::VolDown:
        return "VolDown";
    case bsp::KeyCodes::Torch:
        return "Torch";
    case bsp::KeyCodes::SSwitchUp:
        return "SSwitchUp";
    case bsp::KeyCodes::SSwitchDown:
        return "SSwitchDown";
    case bsp::KeyCodes::SSwitchMid:
        return "SSwitchMid";
    }
    return "";
}
