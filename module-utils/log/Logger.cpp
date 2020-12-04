// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "critical.hpp"
#include <fstream>
#include <gsl/gsl_util>
#include "Logger.hpp"
#include "macros.h"

namespace Log
{
    std::map<std::string, logger_level> Logger::filtered = {{"ApplicationManager", logger_level::LOGINFO},
                                                            {"TS0710Worker", logger_level::LOGINFO},
                                                            {"ServiceCellular", logger_level::LOGINFO},
                                                            {"ServiceAntenna", logger_level::LOGINFO},
                                                            {"ServiceFota", logger_level::LOGINFO},
                                                            {"ServiceEink", logger_level::LOGINFO},
                                                            {"ServiceDB", logger_level::LOGINFO},
                                                            {CRIT_STR, logger_level::LOGTRACE},
                                                            {IRQ_STR, logger_level::LOGTRACE}};
    const char *Logger::level_names[]                    = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    auto Logger::GetLogLevel(const std::string &name) -> logger_level
    {
        return filtered[name];
    }

    bool Logger::lock()
    {
        if (isIRQ()) {
            bt = cpp_freertos::CriticalSection::EnterFromISR();
        }
        else {
            return mutex.Lock();
        }
        return true;
    }

    auto Logger::log(Device device, const char *fmt, va_list args) -> int
    {
        if (!lock()) {
            return -1;
        }
        auto _ = gsl::finally([this] { unlock(); });

        loggerBufferCurrentPos = 0;
        loggerBufferCurrentPos += vsnprintf(&loggerBuffer[loggerBufferCurrentPos], loggerBufferSizeLeft(), fmt, args);
        logToDevice(device, loggerBuffer, loggerBufferCurrentPos);

        return loggerBufferCurrentPos;
    }

    void Logger::log(
        logger_level level, const char *file, int line, const char *function, const char *fmt, va_list args)
    {
        if (!filterLogs(level)) {
            return;
        }

        if (!lock()) {
            return;
        }
        auto _ = gsl::finally([this] { unlock(); });

        loggerBufferCurrentPos = 0;
        addLogHeader(level, file, line, function);

        loggerBufferCurrentPos += vsnprintf(&loggerBuffer[loggerBufferCurrentPos], loggerBufferSizeLeft(), fmt, args);
        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos], loggerBufferSizeLeft(), "\n");

        logToDevice(Device::DEFAULT, loggerBuffer, loggerBufferCurrentPos);
    }

    auto Logger::logAssert(const char *fmt, va_list args) -> int
    {
        if (!lock()) {
            return -1;
        }
        auto _ = gsl::finally([this] { unlock(); });

        logToDevice(fmt, args);

        return loggerBufferCurrentPos;
    }

    void Logger::unlock()
    {
        if (isIRQ()) {
            cpp_freertos::CriticalSection::ExitFromISR(bt);
        }
        else {
            mutex.Unlock();
        }
    }
}; // namespace Log
