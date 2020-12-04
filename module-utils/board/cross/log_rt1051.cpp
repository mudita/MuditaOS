// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <board.h>
#include <critical.hpp>
#include "log/log.hpp"
#include "log/Logger.hpp"
#include "segger/rtt/SEGGER_RTT.h"
#include <ticks.hpp>

/*
 * TODO: M.P
 * Redictering log output to serial console in this form need be considered as quick&dirty solution.
 * It should be refactored i.e serial terminal BSP created and used here instead of using RT1051's low-level routines
 * directly.
 */

extern const char *level_names[];

extern Log::Logger logger;

/// get string description:
/// - in critical seciton - return CRIT
/// - in interrupt return - IRQ
/// - else return thread name
static inline const char *getTaskDesc()
{
    return xTaskGetCurrentTaskHandle() == NULL
               ? Log::Logger::CRIT_STR
               : xPortIsInsideInterrupt() ? Log::Logger::IRQ_STR : pcTaskGetName(xTaskGetCurrentTaskHandle());
}

namespace Log
{
    void Logger::addLogHeader(logger_level level, const char *file, int line, const char *function)
    {
        _loggerBufferCurrentPos += snprintf(&_loggerBuffer[_loggerBufferCurrentPos],
                                            loggerBufferSizeLeft(),
                                            "%lu ms ",
                                            cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));

        _loggerBufferCurrentPos += snprintf(&_loggerBuffer[_loggerBufferCurrentPos],
                                            loggerBufferSizeLeft(),
                                            "%-5s [%s] %s:%s:%d: ",
                                            level_names[level],
                                            getTaskDesc(),
                                            file,
                                            function,
                                            line);
    }

    bool Logger::filterLogs(logger_level level)
    {
        return filterThreadName(getTaskDesc()) < level;
    }

    bool Logger::lock()
    {
        // if called from the ISR use DI/EI.
        if (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) {
            _bt = cpp_freertos::CriticalSection::EnterFromISR();
        }
        else {
            return _mutex.Lock();
        }
        return true;
    }

    auto Logger::logAssert(const char *fmt, va_list args) -> int
    {
        if (!lock()) {
            return -1;
        }

        const size_t startingPos = _loggerBufferCurrentPos;
        _loggerBufferCurrentPos += snprintf(&_loggerBuffer[_loggerBufferCurrentPos],
                                            loggerBufferSizeLeft(),
                                            "%lu ms ",
                                            cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));

        _loggerBufferCurrentPos += snprintf(&_loggerBuffer[_loggerBufferCurrentPos],
                                            loggerBufferSizeLeft(),
                                            "%-5s [%-10s] \x1b[31mASSERTION ",
                                            level_names[LOGFATAL],
                                            getTaskDesc());

        _loggerBufferCurrentPos +=
            vsnprintf(&_loggerBuffer[_loggerBufferCurrentPos], loggerBufferSizeLeft(), fmt, args);
        const auto length = _loggerBufferCurrentPos - startingPos;
        logToDevice(Device::DEFAULT, &_loggerBuffer[startingPos], length);
        dumpToFile();

        unlock();
        return length;
    }

    void Logger::logToDevice(Device device, std::string_view log, size_t length)
    {
        switch (device) {
        case Device::DEFAULT:
            log_WriteToDevice(reinterpret_cast<const uint8_t *>(log.data()), length);
            break;
        case Device::SEGGER_RTT:
            SEGGER_RTT_Write(0, reinterpret_cast<const void *>(log.data()), length);
            break;
        default:
            break;
        }
    }

    void Logger::unlock()
    {
        // if called from the ISR use DI/EI.
        if (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) {
            cpp_freertos::CriticalSection::ExitFromISR(_bt);
        }
        else {
            _mutex.Unlock();
        }
    }
} // namespace Log

extern "C"
{
    int printf(__const char *__restrict __format, ...)
    {
        va_list args;
        va_start(args, __format);
        const auto result = logger.log(Log::Device::SEGGER_RTT, __format, args);
        va_end(args);
        return result;
    }

    int vprintf(const char *__restrict __format, va_list __arg)
    {
        return logger.log(Log::Device::SEGGER_RTT, __format, __arg);
    }

    int log_assert(__const char *__restrict __format, ...)
    {
        va_list args;
        va_start(args, __format);
        const auto result = logger.logAssert(__format, args);
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
