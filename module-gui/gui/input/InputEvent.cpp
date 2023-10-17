// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputEvent.hpp"
#include <gsl/assert>

namespace gui
{

    InputEvent::InputEvent(RawKey key, State state, KeyCode keyCode) : rawKey(key), state(state), keyCode(keyCode)
    {}

    std::string InputEvent::str() const
    {
        std::stringstream ss;
        ss << "KeyCode: " << c_str(keyCode) << " ";
        ss << "State: " << c_str(state) << " ";
        ss << "short press: " << isShortRelease();
        return ss.str();
    }

    auto InputEvent::numericValue() const -> int
    {
        Expects(isDigit());
        return toNumeric(keyCode);
    }

}; // namespace gui
