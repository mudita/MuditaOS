// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/api/request/request.hpp>
#include <at/SimState.hpp>

namespace cellular::internal::msg
{
    struct SimStateChanged : public cellular::msg::Request
    {
        SimStateChanged(at::SimState state) : state(state)
        {}
        at::SimState state;
    };
} // namespace cellular::internal::msg
