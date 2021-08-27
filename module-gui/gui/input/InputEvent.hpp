// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <sstream>
#include <cstdint>
#include <type_traits>
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

        HEADSET_OK    = static_cast<int>(bsp::KeyCodes::HeadsetOk),
        HEADSET_VOLUP = static_cast<int>(bsp::KeyCodes::HeadsetVolUp),
        HEADSET_VOLDN = static_cast<int>(bsp::KeyCodes::HeadsetVolDown),
    };

    static const int InvalidNumericKeyCode = -1;

    /// if numeric - returns numeric value, else return: InvalidNumericKeyCode ( -1 )
    [[nodiscard]] inline auto toNumeric(KeyCode key) -> int
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
            return InvalidNumericKeyCode;
        }
    }

    class InputEvent
    {
      public:
        enum class State
        {
            Undefined        = 0x00, /// No action defined or translation error
            keyPressed       = 0x01, /// Key pressed event
            keyReleasedShort = 0x02, /// Key released before timeout
            keyReleasedLong  = 0x04, /// Key released after timeout
            keyMoved         = 0x05, /// Monostable key event
        };

        InputEvent(RawKey key, State state = State::Undefined, KeyCode keyCode = KeyCode::KEY_UNDEFINED);

        [[nodiscard]] auto getRawKey() const -> RawKey
        {
            return rawKey;
        }

        [[nodiscard]] auto getState() const -> State
        {
            return state;
        }

        void setState(State s)
        {
            state = s;
        }

        [[nodiscard]] auto getKeyCode() const -> KeyCode
        {
            return keyCode;
        }

        void setKeyCode(KeyCode code)
        {
            keyCode = code;
        }

        [[nodiscard]] auto isKeyPress() const -> bool
        {
            return state == State::keyPressed;
        }

        [[nodiscard]] auto isKeyPress(KeyCode code) const -> bool
        {
            return isKeyPress() && is(code);
        }

        [[nodiscard]] auto isShortRelease() const -> bool
        {
            return state == State::keyReleasedShort;
        }

        [[nodiscard]] auto isShortRelease(KeyCode code) const -> bool
        {
            return isShortRelease() && is(code);
        }

        [[nodiscard]] auto isLongRelease() const -> bool
        {
            return state == State::keyReleasedLong;
        }

        [[nodiscard]] auto isLongRelease(KeyCode code) const -> bool
        {
            return isLongRelease() && is(code);
        }

        [[nodiscard]] auto isKeyRelease() const -> bool
        {
            return isShortRelease() || isLongRelease();
        }

        [[nodiscard]] auto isKeyRelease(KeyCode code) const -> bool
        {
            return isKeyRelease() && is(code);
        }

        [[nodiscard]] auto is(KeyCode code) const -> bool
        {
            return keyCode == code;
        }

        [[nodiscard]] auto isDigit() const -> bool
        {
            return toNumeric(keyCode) != InvalidNumericKeyCode;
        }

        [[nodiscard]] auto str() const -> std::string;

        [[nodiscard]] auto numericValue() const -> int;

      private:
        RawKey rawKey   = {};                     /// RawKey data
        State state     = State::keyPressed;      /// initial translated key state
        KeyCode keyCode = KeyCode::KEY_UNDEFINED; /// initial translated key code
    };

    static_assert(std::is_trivially_copyable_v<InputEvent>);

} // namespace gui

[[nodiscard]] inline auto c_str(gui::InputEvent::State state) -> const char *
{
    switch (state) {
    case gui::InputEvent::State::Undefined:
        return "Undefined";
    case gui::InputEvent::State::keyPressed:
        return "keyPressed";
    case gui::InputEvent::State::keyReleasedShort:
        return "keyReleasedShort";
    case gui::InputEvent::State::keyReleasedLong:
        return "keyReleasedLong ";
    case gui::InputEvent::State::keyMoved:
        return "keyMoved";
    }
    return "";
}

[[nodiscard]] inline auto c_str(gui::KeyCode key) -> const char *
{
    switch (key) {
    case gui::KeyCode::KEY_UNDEFINED:
        return "KEY_UNDEFINED";
    case gui::KeyCode::KEY_LEFT:
        return "KEY_LEFT";
    case gui::KeyCode::KEY_UP:
        return "KEY_UP";
    case gui::KeyCode::KEY_RIGHT:
        return "KEY_RIGHT";
    case gui::KeyCode::KEY_DOWN:
        return "KEY_DOWN";
    case gui::KeyCode::KEY_LF:
        return "KEY_LF";
    case gui::KeyCode::KEY_RF:
        return "KEY_RF";
    case gui::KeyCode::KEY_ENTER:
        return "KEY_ENTER";
    case gui::KeyCode::KEY_0:
        return "KEY_0";
    case gui::KeyCode::KEY_1:
        return "KEY_1";
    case gui::KeyCode::KEY_2:
        return "KEY_2";
    case gui::KeyCode::KEY_3:
        return "KEY_3";
    case gui::KeyCode::KEY_4:
        return "KEY_4";
    case gui::KeyCode::KEY_5:
        return "KEY_5";
    case gui::KeyCode::KEY_6:
        return "KEY_6";
    case gui::KeyCode::KEY_7:
        return "KEY_7";
    case gui::KeyCode::KEY_8:
        return "KEY_8";
    case gui::KeyCode::KEY_9:
        return "KEY_9";
    case gui::KeyCode::KEY_AST:
        return "KEY_AST";
    case gui::KeyCode::KEY_PND:
        return "KEY_PND";
    case gui::KeyCode::KEY_VOLUP:
        return "KEY_VOLUP";
    case gui::KeyCode::KEY_VOLDN:
        return "KEY_VOLDN";
    case gui::KeyCode::KEY_TORCH:
        return "KEY_TORCH";
    case gui::KeyCode::SWITCH_UP:
        return "SWITCH_UP";
    case gui::KeyCode::SWITCH_MID:
        return "SWITCH_MID";
    case gui::KeyCode::SWITCH_DN:
        return "SWITCH_DN";
    case gui::KeyCode::HEADSET_OK:
        return "HEADSET_OK";
    case gui::KeyCode::HEADSET_VOLUP:
        return "HEADSET_VOLUP";
    case gui::KeyCode::HEADSET_VOLDN:
        return "HEADSET_VOLDN";
    }
    return "";
}
