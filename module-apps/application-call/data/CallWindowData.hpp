// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CallWindowState.hpp"
#include <SwitchData.hpp>

namespace gui
{
    class CallWindowData : public SwitchData
    {
        call::State state;

      public:
        CallWindowData(call::State state) : state(state)
        {}
        [[nodiscard]] const auto &getState() const noexcept
        {
            return state;
        }
    };
} // namespace gui
