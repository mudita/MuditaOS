
/*
 * @file key_codes_rt1051.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 22.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_KEY_CODES_RT1051_HPP
#define PUREPHONE_KEY_CODES_RT1051_HPP

namespace bsp {

    enum class KeyCodes {
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

#endif //PUREPHONE_KEY_CODES_RT1051_HPP
