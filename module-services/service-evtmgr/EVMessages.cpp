// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/EVMessages.hpp"

namespace sevm
{
    bool KeypadBacklightMessage::processAction(bsp::keypad_backlight::Action act)
    {
        bool response = false;
        switch (act) {
        case bsp::keypad_backlight::Action::turnOn:
            response = bsp::keypad_backlight::turnOnAll();
            break;
        case bsp::keypad_backlight::Action::turnOff:
            response = bsp::keypad_backlight::shutdown();
            break;
        case bsp::keypad_backlight::Action::checkState:
            response = bsp::keypad_backlight::checkState();
            break;
        }
        return response;
    }
} // namespace sevm
