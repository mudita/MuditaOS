#include "Bluetooth.hpp"
#include <cstdarg>

namespace bsp
{
    BTDevice::BTDevice() : logLevel{LogNone}, isOpen{false}, logFunction{nullptr}
    {}

    void BTDevice::log(LogLevel level, const char *val, ...)
    {
        if ((logLevel >= level) && (logFunction != nullptr)) {
            va_list args;
            va_start(args, val);
            logFunction(val, args);
            va_end(args);
        }
    }
}
