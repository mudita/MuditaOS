// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <macros.h>
#include <log/log.hpp>
#include <Logger.hpp>
#include <SEGGER_RTT.h>
#include <ticks.hpp>

namespace Log
{
    void Logger::logToDevice(const char *fmt, va_list args)
    {
        lineBufferCurrentPos = 0;

        auto bufferSizeLeft = lineBufferSizeLeft();
        auto bytesParsed    = snprintf(&lineBuffer[lineBufferCurrentPos],
                                    bufferSizeLeft,
                                    "%lu ms ",
                                    cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));
        if (bytesParsed >= 0) {
            lineBufferCurrentPos += std::min(bufferSizeLeft, static_cast<std::size_t>(bytesParsed));
        }

        bufferSizeLeft = lineBufferSizeLeft();
        bytesParsed    = snprintf(&lineBuffer[lineBufferCurrentPos],
                               bufferSizeLeft,
                               "%-5s [%-10s] \x1b[31mASSERTION ",
                               levelNames[LOGFATAL],
                               getTaskDesc());
        if (bytesParsed >= 0) {
            lineBufferCurrentPos += std::min(bufferSizeLeft, static_cast<std::size_t>(bytesParsed));
        }

        bufferSizeLeft = lineBufferSizeLeft();
        bytesParsed    = vsnprintf(&lineBuffer[lineBufferCurrentPos], bufferSizeLeft, fmt, args);
        if (bytesParsed >= 0) {
            lineBufferCurrentPos += std::min(bufferSizeLeft, static_cast<std::size_t>(bytesParsed));
        }

        logToDevice(Device::DEFAULT, lineBuffer, lineBufferCurrentPos);
        buffer.getCurrentBuffer()->put(std::string(lineBuffer, lineBufferCurrentPos));
    }

    void Logger::logToDevice(Device device, const char *logMsg, std::size_t length)
    {
        switch (device) {
        case Device::DEFAULT:
            log_WriteToDevice(reinterpret_cast<const uint8_t *>(logMsg), length);
            break;
        case Device::SEGGER_RTT:
            SEGGER_RTT_Write(0, logMsg, length);
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
