// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <cstdarg>
__attribute__((weak)) void log_Log(
    logger_level level, const char *file, int line, const char *function, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    va_end(args);
}

__attribute__((weak)) void log_Printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    va_end(args);
}
