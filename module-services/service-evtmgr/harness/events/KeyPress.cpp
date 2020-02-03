#include "KeyPress.hpp"
#include "../../messages/EVMessages.hpp"
#include "../Events.hpp"

namespace harness
{

    bsp::KeyCodes fromVal(int val);

    std::shared_ptr<sys::DataMessage> parseKeyPress(json11::Json &js)
    {
        auto message = std::make_shared<sevm::KbdMessage>(MessageType::KBDKeyEvent);
        if (!js[Data].is_array())
        {
            LOG_ERROR("bad json format, data not array: %s", js.dump().c_str());
            return std::make_shared<sys::DataMessage>(false);
        }
        message->key.key_code = fromVal(js[Data][0].int_value());
        /// for now type is only released
        message->key.state = RawKey::State::Released;
        return message;
    }

    bsp::KeyCodes fromVal(int val)
    {
        switch (val)
        {
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
}; // namespace harness
