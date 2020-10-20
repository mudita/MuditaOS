// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/watchdog/watchdog.hpp>

namespace bsp::watchdog
{
    __attribute__((weak)) void init()
    {}
    __attribute__((weak)) void system_reset()
    {}
    __attribute__((weak)) void pet()
    {}
    __attribute__((weak)) std::string reset_cause()
    {
        return {};
    }
} // namespace bsp::watchdog
