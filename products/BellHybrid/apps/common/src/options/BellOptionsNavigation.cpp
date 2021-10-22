// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "common/options/BellOptionsNavigation.hpp"

namespace gui
{
    InputEvent invertNavigationDirection(const InputEvent &inputEvent)
    {
        InputEvent input = inputEvent;
        if (input.is(KeyCode::KEY_UP)) {
            input.setKeyCode(KeyCode::KEY_DOWN);
        }
        else if (input.is(KeyCode::KEY_DOWN)) {
            input.setKeyCode(KeyCode::KEY_UP);
        }
        return input;
    }
}; // namespace gui
