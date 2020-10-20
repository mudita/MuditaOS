// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputEvent.hpp"
namespace gui
{

    InputEvent::InputEvent(RawKey key, State state, KeyCode keyCode) : key(key), state(state), keyCode(keyCode)
    {}

    std::string InputEvent::str() const
    {
        std::stringstream ss;
        ss << "KeyCode: " << c_str(keyCode) << " ";
        ss << "State: " << c_str(state) << " ";
        ss << "short press: " << isShortPress();
        return ss.str();
    }

}; // namespace gui
