#include "watchdog.hpp"

namespace bsp::watchdog
{
    __attribute__((weak)) void init() {}
    __attribute__((weak)) void system_reset() {}
    __attribute__((weak)) void pet() {}
    __attribute__((weak)) std::string reset_cause() { return {}; }
}
