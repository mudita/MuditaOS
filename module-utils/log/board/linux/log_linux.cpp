// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <Logger.hpp>
#include <iostream>
#include <ticks.hpp>

namespace Log
{
    void Logger::logToDevice(const char *, va_list)
    {
        assert(false && "Not implemented");
    }

    void Logger::logToDevice(Device, const char *logMsg, size_t)
    {
        std::cout << logMsg;
    }
} // namespace Log
