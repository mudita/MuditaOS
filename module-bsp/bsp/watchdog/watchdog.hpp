#pragma once

#include <string>

namespace bsp::watchdog
{
    /// initializes and starts watchdog
    /// - for linux             - does nothing
    /// - for rt1051 debug      - initializes with maximum value (125 sec)
    /// - for rt1051 release    - initializes with maximum value (125 sec)
    void init();
    /// resets system
    /// - for linux - does nothing - consider just exit(0)
    void system_reset();
    /// update watchdog, so that it won't kick us
    void pet();
    /// get watchdog reset cause
    std::string reset_cause();
}
