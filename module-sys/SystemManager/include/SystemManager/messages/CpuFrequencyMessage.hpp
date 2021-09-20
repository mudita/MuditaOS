// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SystemManagerMessage.hpp"

namespace sys
{
    class CpuFrequencyMessage : public SystemManagerMessage
    {
      public:
        enum class Action
        {
            Increase,
            Decrease
        };

        CpuFrequencyMessage(Action action) : SystemManagerMessage(), action(action)
        {}

        [[nodiscard]] auto getAction() const noexcept
        {
            return action;
        };

      private:
        Action action = Action::Increase;
    };
} // namespace sys
