// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>
#include <Logger.hpp>
#include <iostream>
#include <string_view>
#include <ticks.hpp>

namespace Log
{
    void Logger::addLogHeader(logger_level level, const char *file, int line, const char *function)
    {
        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           LOGGER_BUFFER_SIZE - loggerBufferCurrentPos,
                                           "%d ms ",
                                           cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));

        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           LOGGER_BUFFER_SIZE - loggerBufferCurrentPos,
                                           "%s%-5s %s%s:%s:%d:%s ",
                                           logColors->levelColors[level].data(),
                                           levelNames[level],
                                           logColors->callerInfoColor.data(),
                                           file,
                                           function,
                                           line,
                                           logColors->resetColor.data());
    }

    bool Logger::filterLogs(logger_level _level)
    {
        return _level >= level;
    }

    void Logger::logToDevice(const char *, va_list)
    {
        assert(false && "Not implemented");
    }

    void Logger::logToDevice(Device, std::string_view logMsg, size_t)
    {
        std::cout << logMsg;
    }
} // namespace Log
