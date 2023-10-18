// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Label.hpp"

namespace style
{
    namespace buttonTriState
    {
        inline constexpr std::uint32_t w                         = 56;
        inline constexpr std::uint32_t h                         = 32;
        inline constexpr std::uint32_t penWidth                  = 2;
        inline constexpr std::uint32_t cornerRadiusRectangleView = 4;
        inline constexpr std::uint32_t cornerRadiusTextView      = 0;
    } // namespace buttonTriState
} // namespace style

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
        const std::string transitingText;
        State currentState;
    };
} // namespace gui
