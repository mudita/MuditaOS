// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "log/log.hpp"
#include "log/Logger.hpp"
#include <fstream>
#include <iostream>
#include <string_view>
#include <ticks.hpp>

extern const char *level_names[];

#if LOG_USE_COLOR == 1
static const char *level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif

namespace Log
{
    void Logger::addLogHeader(logger_level level, const char *file, int line, const char *function)
    {
        _loggerBufferCurrentPos += snprintf(&_loggerBuffer[_loggerBufferCurrentPos],
                                            LOGGER_BUFFER_SIZE - _loggerBufferCurrentPos,
                                            "%d ms ",
                                            cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));
#if LOG_USE_COLOR == 1
        _loggerBufferCurrentPos += snprintf(&_loggerBuffer[_loggerBufferCurrentPos],
                                            LOGGER_BUFFER_SIZE - _loggerBufferCurrentPos,
                                            "%s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
                                            level_colors[level],
                                            level_names[level],
                                            file,
                                            line);
#else
        _loggerBufferCurrentPos += snprintf(&_loggerBuffer[_loggerBufferCurrentPos],
                                            LOGGER_BUFFER_SIZE - _loggerBufferCurrentPos,
                                            "%-5s %s:%s:%d: ",
                                            level_names[level],
                                            file,
                                            function,
                                            line);
#endif
    }

    bool Logger::filterLogs(logger_level level)
    {
        return level >= _level;
    }

    bool Logger::lock()
    {
        return _mutex.try_lock();
    }

    void Logger::logToDevice(Device, std::string_view log, size_t)
    {
        std::cout << log;
    }

    void Logger::unlock()
    {
        _mutex.unlock();
    }

} // namespace Log
