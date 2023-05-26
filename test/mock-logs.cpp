// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <cstdarg>
__attribute__((weak)) int log_Log(
    LoggerLevel level, const char *file, int line, const char *function, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    va_end(args);
    return 0;
}

__attribute__((weak)) int log_Printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    va_end(args);
    return 0;
}
