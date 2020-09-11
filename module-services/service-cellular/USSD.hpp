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
        pushSesion,
        pullRequestSent,
        pullResponseReceived,
        sesionAborted
    };
} // namespace ussd
