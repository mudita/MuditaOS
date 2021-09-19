// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <board/board.h>
#include <critical.hpp>
#include <macros.h>
#include <log.hpp>
#include <Logger.hpp>
#include <SEGGER_RTT.h>
#include <ticks.hpp>

namespace Log
{
    void Logger::logToDevice(const char *fmt, va_list args)
    {
        loggerBufferCurrentPos = 0;
        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           loggerBufferSizeLeft(),
                                           "%lu ms ",
                                           cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));

        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           loggerBufferSizeLeft(),
                                           "%-5s [%-10s] \x1b[31mASSERTION ",
                                           levelNames[LOGFATAL],
                                           getTaskDesc());

        loggerBufferCurrentPos += vsnprintf(&loggerBuffer[loggerBufferCurrentPos], loggerBufferSizeLeft(), fmt, args);
        logToDevice(Device::DEFAULT, loggerBuffer, loggerBufferCurrentPos);
        circularBuffer.put(std::string(loggerBuffer, loggerBufferCurrentPos));
    }

    void Logger::logToDevice(Device device, std::string_view logMsg, size_t length)
    {
        switch (device) {
        case Device::DEFAULT:
            log_WriteToDevice(reinterpret_cast<const uint8_t *>(logMsg.data()), length);
            break;
        case Device::SEGGER_RTT:
            SEGGER_RTT_Write(0, reinterpret_cast<const void *>(logMsg.data()), length);
            break;
        default:
            break;
        }
    }
} // namespace Log

using Log::Logger;

extern "C"
{
    int printf(__const char *__restrict __format, ...)
    {
        va_list args;
        va_start(args, __format);
        const auto result = Logger::get().log(Log::Device::SEGGER_RTT, __format, args);
        va_end(args);
        return result;
    }

    int vprintf(const char *__restrict __format, va_list __arg)
    {
        return Logger::get().log(Log::Device::SEGGER_RTT, __format, __arg);
    }

    int log_assert(__const char *__restrict __format, ...)
    {
        va_list args;
        va_start(args, __format);
        const auto result = Logger::get().logAssert(__format, args);
        va_end(args);
        return result;
    }

    void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
    {
        log_assert("\"%s\" failed: file \"%s\", line %d%s%s\n",
                   failedexpr,
                   file,
                   line,
                   func ? ", function: " : "",
                   func ? func : "");

        abort();
        /* NOTREACHED */
    }
};
