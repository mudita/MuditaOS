// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <service-cellular/api/message.hpp>
#include <at/SimState.hpp>

namespace cellular::internal::msg
{
    struct HandleATSimStateChange : public cellular::msg::Request
    {
        HandleATSimStateChange(at::SimState state) : state(state)
        {}
        const at::SimState state;
    };
} // namespace cellular::internal::msg
