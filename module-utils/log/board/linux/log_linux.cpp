// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <Logger.hpp>
#include <iostream>
#include <string_view>
#include <ticks.hpp>

namespace Log
{
    void Logger::logToDevice(const char *, va_list)
    {
        assert(false && "Not implemented");
    }

    void Logger::logToDevice(Device, std::string_view logMsg, size_t)
    {
        std::cout << logMsg;
    }
} // namespace Log
