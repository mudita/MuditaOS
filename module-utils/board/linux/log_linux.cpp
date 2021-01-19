// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "log/log.hpp"
#include "log/Logger.hpp"
#include <iostream>
#include <string_view>
#include <ticks.hpp>

#if LOG_USE_COLOR == 1
static const char *level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif

namespace Log
{
    void Logger::addLogHeader(logger_level level, const char *file, int line, const char *function)
    {
        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           LOGGER_BUFFER_SIZE - loggerBufferCurrentPos,
                                           "%d ms ",
                                           cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));
#if LOG_USE_COLOR == 1
        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           LOGGER_BUFFER_SIZE - loggerBufferCurrentPos,
                                           "%s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
                                           level_colors[level],
                                           level_names[level],
                                           file,
                                           line);
#else
        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           LOGGER_BUFFER_SIZE - loggerBufferCurrentPos,
                                           "%-5s %s:%s:%d: ",
                                           level_names[level],
                                           file,
                                           function,
                                           line);
#endif
    }

    bool Logger::filterLogs(logger_level _level)
    {
        return _level >= level;
    }

    void Logger::logToDevice(const char *, va_list)
    {
        assert(false && "Not implemented");
    }

    void Logger::logToDevice(Device, std::string_view log, size_t)
    {
        std::cout << log;
    }
} // namespace Log
