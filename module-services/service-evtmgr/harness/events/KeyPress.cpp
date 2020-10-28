// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "KeyPress.hpp"

#include <map>    // for map<>::mapped_type
#include <memory> // for make_shared, shared_ptr, __shared_ptr_access

#include "../Events.hpp"              // for Data, Type
#include "bsp/keyboard/key_codes.hpp" // for KeyCodes, KeyCodes::FnLeft, KeyCodes::FnRight, KeyCodes::JoystickDown, KeyCodes::JoystickEnter, KeyCodes::JoystickLeft, KeyCodes::JoystickRight, KeyCodes::JoystickUp, KeyCodes::NumericKey0, KeyCodes::NumericKey1, KeyCodes::NumericKey2, KeyCodes::NumericKey3, KeyCodes::NumericKey4, KeyCodes::NumericKey5, KeyCodes::NumericKey6, KeyCodes::NumericKey7, KeyCodes::NumericKey8, KeyCodes::NumericKey9, KeyCodes::NumericKeyAst, KeyCodes::NumericKeyPnd, KeyCodes::SSwitchDown, KeyCodes::SSwitchMid, KeyCodes::SSwitchUp, KeyCodes::Torch, KeyCodes::Undefined, KeyCodes::VolDown, KeyCodes::VolUp
#include "common_data/RawKey.hpp"     // for RawKey, RawKey::State, RawKey::State::Released
#include "service-evtmgr/messages/KbdMessage.hpp" // for KbdMessage

namespace harness::events
{

    using namespace json11;

    KeyPress::KeyPress(const Json &js)
    {
        state = decode(js);
    }

    auto fromVal(int val) -> bsp::KeyCodes;

    auto KeyPress::decode(const Json &js) -> bool
    {
        auto message = std::make_shared<sevm::KbdMessage>();
        msg          = message;
        if (js[Data].is_array() != true) {
            return false;
        }
        message->key.key_code = fromVal(js[Data][0].int_value());
        /// for now type is only released
        message->key.state = RawKey::State::Released;
        return true;
    }

    auto KeyPress::encode() -> std::string
    {
        Json::object el;
        el[Type] = (int)evt;
        el[Data] = {};
        return Json(el).dump();
    }

    auto fromVal(int val) -> bsp::KeyCodes
    {
        switch (val) {
        case 0:
            return bsp::KeyCodes::NumericKey0;
        case 1:
            return bsp::KeyCodes::NumericKey1;
        case 2:
            return bsp::KeyCodes::NumericKey2;
        case 3:
            return bsp::KeyCodes::NumericKey3;
        case 4:
            return bsp::KeyCodes::NumericKey4;
        case 5:
            return bsp::KeyCodes::NumericKey5;
        case 6:
            return bsp::KeyCodes::NumericKey6;
        case 7:
            return bsp::KeyCodes::NumericKey7;
        case 8:
            return bsp::KeyCodes::NumericKey8;
        case 9:
            return bsp::KeyCodes::NumericKey9;
        case '*':
            return bsp::KeyCodes::NumericKeyAst;
        case '#':
            return bsp::KeyCodes::NumericKeyPnd;
        case 'a':
            return bsp::KeyCodes::JoystickLeft;
        case 'd':
            return bsp::KeyCodes::JoystickRight;
        case 'w':
            return bsp::KeyCodes::JoystickUp;
        case 's':
            return bsp::KeyCodes::JoystickDown;
        case '\n':
            return bsp::KeyCodes::JoystickEnter;
        case 11:
            return bsp::KeyCodes::FnLeft;
        case 12:
            return bsp::KeyCodes::FnRight;
        case 13:
            return bsp::KeyCodes::VolUp;
        case 14:
            return bsp::KeyCodes::VolDown;
        case 15:
            return bsp::KeyCodes::Torch;
        case 16:
            return bsp::KeyCodes::SSwitchUp;
        case 17:
            return bsp::KeyCodes::SSwitchDown;
        case 18:
            return bsp::KeyCodes::SSwitchMid;
        default:
            return bsp::KeyCodes::Undefined;
        };
    }
}; // namespace harness::events
