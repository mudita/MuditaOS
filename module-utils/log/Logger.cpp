// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "critical.hpp"
#include <fstream>
#include <gsl/gsl_util>
#include "LockGuard.hpp"
#include "Logger.hpp"
#include "macros.h"

namespace Log
{
    std::map<std::string, logger_level> Logger::filtered = {{"ApplicationManager", logger_level::LOGINFO},
                                                            {"TS0710Worker", logger_level::LOGINFO},
                                                            {"ServiceCellular", logger_level::LOGINFO},
                                                            {"ServiceAntenna", logger_level::LOGINFO},
                                                            {"ServiceAudio", logger_level::LOGINFO},
                                                            {"ServiceBluetooth", logger_level::LOGINFO},
                                                            {"ServiceFota", logger_level::LOGINFO},
                                                            {"ServiceEink", logger_level::LOGINFO},
                                                            {"ServiceDB", logger_level::LOGINFO},
                                                            {CRIT_STR, logger_level::LOGTRACE},
                                                            {IRQ_STR, logger_level::LOGTRACE}};
    const char *Logger::levelNames[]                     = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    Logger::Logger() : circularBuffer(circularBufferSize)
    {}

    void Logger::enableColors(bool enable)
    {
        LockGuard lock(mutex);

        if (enable) {
            logColors = &logColorsOn;
        }
        else {
            logColors = &logColorsOff;
        }
    }

    auto Logger::getLogLevel(const std::string &name) -> logger_level
    {
        return filtered[name];
    }

    auto Logger::getLogs() -> std::string
    {
        LockGuard lock(mutex);

        std::string logs;
        while (!circularBuffer.isEmpty()) {
            const auto [result, msg] = circularBuffer.get();
            if (result) {
                logs += msg;
            }
        }
        return logs;
    }

    void Logger::init()
    {
#if LOG_USE_COLOR == 1
        enableColors(true);
#else
        enableColors(false);
#endif
    }

    auto Logger::log(Device device, const char *fmt, va_list args) -> int
    {
        LockGuard lock(mutex);

        loggerBufferCurrentPos = 0;

        const auto sizeLeft = loggerBufferSizeLeft();
        const auto result   = vsnprintf(&loggerBuffer[loggerBufferCurrentPos], sizeLeft, fmt, args);
        if (0 <= result) {
            const auto numOfBytesAddedToBuffer = static_cast<size_t>(result);
            loggerBufferCurrentPos += (numOfBytesAddedToBuffer < sizeLeft) ? numOfBytesAddedToBuffer : (sizeLeft - 1);

            logToDevice(device, loggerBuffer, loggerBufferCurrentPos);
            circularBuffer.put(std::string(loggerBuffer, loggerBufferCurrentPos));
            return loggerBufferCurrentPos;
        }
        return -1;
    }

    auto Logger::log(
        logger_level level, const char *file, int line, const char *function, const char *fmt, va_list args) -> int
    {
        if (!filterLogs(level)) {
            return -1;
        }
        LockGuard lock(mutex);

        loggerBufferCurrentPos = 0;
        addLogHeader(level, file, line, function);

        const auto sizeLeft = loggerBufferSizeLeft();
        const auto result   = vsnprintf(&loggerBuffer[loggerBufferCurrentPos], sizeLeft, fmt, args);
        if (0 <= result) {
            const auto numOfBytesAddedToBuffer = static_cast<size_t>(result);
            loggerBufferCurrentPos += (numOfBytesAddedToBuffer < sizeLeft) ? numOfBytesAddedToBuffer : (sizeLeft - 1);
            loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos], loggerBufferSizeLeft(), "\n");

            logToDevice(Device::DEFAULT, loggerBuffer, loggerBufferCurrentPos);
            circularBuffer.put(std::string(loggerBuffer, loggerBufferCurrentPos));
            return loggerBufferCurrentPos;
        }
        return -1;
    }

    auto Logger::logAssert(const char *fmt, va_list args) -> int
    {
        LockGuard lock(mutex);

        logToDevice(fmt, args);

        return loggerBufferCurrentPos;
    }
}; // namespace Log
