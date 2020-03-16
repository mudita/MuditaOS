/*
 * @file InputEvent.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_INPUT_INPUTEVENT_HPP_
#define MODULE_GUI_GUI_INPUT_INPUTEVENT_HPP_

#include <sstream>
#include <cstdint>
// module-bsp
#include "bsp/keyboard/key_codes.hpp"
#include "common_data/RawKey.hpp"

namespace gui
{

    enum class KeyCode
    {
        KEY_UNDEFINED = 0,
        KEY_LEFT      = static_cast<int>(bsp::KeyCodes::JoystickLeft),
        KEY_UP        = static_cast<int>(bsp::KeyCodes::JoystickUp),
        KEY_RIGHT     = static_cast<int>(bsp::KeyCodes::JoystickRight),
        KEY_DOWN      = static_cast<int>(bsp::KeyCodes::JoystickDown),
        KEY_LF        = static_cast<int>(bsp::KeyCodes::FnLeft),
        KEY_RF        = static_cast<int>(bsp::KeyCodes::FnRight),
        KEY_ENTER     = static_cast<int>(bsp::KeyCodes::JoystickEnter),
        KEY_0         = static_cast<int>(bsp::KeyCodes::NumericKey0),
        KEY_1         = static_cast<int>(bsp::KeyCodes::NumericKey1),
        KEY_2         = static_cast<int>(bsp::KeyCodes::NumericKey2),
        KEY_3         = static_cast<int>(bsp::KeyCodes::NumericKey3),
        KEY_4         = static_cast<int>(bsp::KeyCodes::NumericKey4),
        KEY_5         = static_cast<int>(bsp::KeyCodes::NumericKey5),
        KEY_6         = static_cast<int>(bsp::KeyCodes::NumericKey6),
        KEY_7         = static_cast<int>(bsp::KeyCodes::NumericKey7),
        KEY_8         = static_cast<int>(bsp::KeyCodes::NumericKey8),
        KEY_9         = static_cast<int>(bsp::KeyCodes::NumericKey9),
        KEY_AST       = static_cast<int>(bsp::KeyCodes::NumericKeyAst),
        KEY_PND       = static_cast<int>(bsp::KeyCodes::NumericKeyPnd),
        //	KEY_GREEN,
        //	KEY_RED,
        KEY_VOLUP  = static_cast<int>(bsp::KeyCodes::VolUp),
        KEY_VOLDN  = static_cast<int>(bsp::KeyCodes::VolDown),
        KEY_TORCH  = static_cast<int>(bsp::KeyCodes::Torch),
        SWITCH_UP  = static_cast<int>(bsp::KeyCodes::SSwitchUp),
        SWITCH_MID = static_cast<int>(bsp::KeyCodes::SSwitchMid),
        SWITCH_DN  = static_cast<int>(bsp::KeyCodes::SSwitchDown),
    };

    /// if numeric - returns numeric value, else return -1
    inline int toNumeric(KeyCode key)
    {
        switch (key) {
        case KeyCode::KEY_0:
            return 0;
        case KeyCode::KEY_1:
            return 1;
        case KeyCode::KEY_2:
            return 2;
        case KeyCode::KEY_3:
            return 3;
        case KeyCode::KEY_4:
            return 4;
        case KeyCode::KEY_5:
            return 5;
        case KeyCode::KEY_6:
            return 6;
        case KeyCode::KEY_7:
            return 7;
        case KeyCode::KEY_8:
            return 8;
        case KeyCode::KEY_9:
            return 9;
        default:
            return -1;
        }
    }

    class InputEvent
    {
      public:
        enum class State
        {
            Undefined        = 0x00,
            keyPressed       = 0x01,
            keyReleasedShort = 0x02,
            keyReleasedLong  = 0x04,
        };

        State state     = State::keyPressed;      /// initial translated key state
        KeyCode keyCode = KeyCode::KEY_UNDEFINED; /// initial translated key code
        RawKey key      = {};                     /// RawKey data

        InputEvent(RawKey key, State state = State::Undefined, KeyCode keyCode = KeyCode::KEY_UNDEFINED);
    };

} // namespace gui

#endif /* MODULE_GUI_GUI_INPUT_INPUTEVENT_HPP_ */
