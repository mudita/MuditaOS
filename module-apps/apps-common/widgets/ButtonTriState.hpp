// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Label.hpp"

namespace style
{
    namespace buttonTriState
    {
        constexpr uint32_t w = 56;
        constexpr uint32_t h = 32;
    }
}

namespace gui
{
    class ButtonTriState : public Label
    {
      public:
        enum class State
        {
            Transiting,
            Off,
            On
        };

        ButtonTriState(Item *parent, State state);
        void switchState(State requestedState);

      private:
        Rect *fill = nullptr;
        State currentState;
    };
}
