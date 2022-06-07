// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace ussd
{

    constexpr uint32_t pullResponseTimeout = 30;
    constexpr uint32_t pullSesionTimeout   = 60;
    constexpr uint32_t noTimeout           = 0;

    enum class State
    {
        none,
        pushSession,
        pullRequestSent,
        pullResponseReceived,
        sessionAborted
    };
} // namespace ussd
