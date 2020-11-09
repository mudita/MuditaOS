// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SystemManagerMessage.hpp"

namespace sys
{
    class ScreenStateMessage : public SystemManagerMessage
    {
      public:
        enum class State
        {
            Lock,
            Unlock
        };

        ScreenStateMessage(State state) : SystemManagerMessage(), state(state)
        {}

        [[nodiscard]] auto getState() const noexcept
        {
            return state;
        };

      private:
        State state = State::Lock;
    };
}; // namespace sys
