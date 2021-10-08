// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Action.hpp"

namespace gui
{

    Action::Action(Item *parent, uint32_t x, uint32_t y) : Image(parent, x, y, 0, 0)
    {
        set(stateMap.at(state));
    }

    void Action::setState(State state)
    {
        if (state == this->state) {
            return;
        }
        this->state = state;
        set(stateMap.at(state));
    }
}; // namespace gui
