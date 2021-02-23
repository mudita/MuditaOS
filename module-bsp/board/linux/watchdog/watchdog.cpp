// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/watchdog/watchdog.hpp"
#include "software_watchdog.hpp"
#include <bsp/watchdog/watchdog.hpp>

namespace bsp::watchdog
{
    static SoftwareWatchdog swWatchdog;

    bool init(unsigned int timeoutMs)
    {
        return swWatchdog.init(static_cast<TickType_t>(timeoutMs));
    }

    void refresh()
    {
        swWatchdog.refresh();
    }
} // namespace bsp::watchdog
