// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Label.hpp"

namespace style
{
    namespace buttonOnOff
    {
        constexpr uint32_t w = 56;
        constexpr uint32_t h = 32;
    } // namespace buttonOnOff
} // namespace style

namespace gui
{
    enum class ButtonState : bool
    {
        Off,
        On
    };

    class ButtonOnOff : public Label
    {
      public:
        ButtonOnOff(Item *parent, const ButtonState buttonState);
        void switchState(const ButtonState);
        void toggleState();

      private:
        ButtonState currentState;
    };

} /* namespace gui */
