// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "log.hpp"
#include "Logger.hpp"
#include <ticks.hpp>

using Log::Logger;

void log_Printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Logger::get().log(fmt, args);
    va_end(args);
}

void log_Log(logger_level level, const char *file, int line, const char *function, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Logger::get().log(level, file, line, function, fmt, args);
    va_end(args);
}

extern "C"
{
    void bt_log_custom(const char *file, int line, const char *foo, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        Logger::get().log(LOGTRACE, file, line, foo, fmt, args);
        va_end(args);
    }
}
