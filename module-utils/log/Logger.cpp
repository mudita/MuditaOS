// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "critical.hpp"
#include <fstream>
#include <gsl/util>
#include "LockGuard.hpp"
#include <Logger.hpp>
#include <functional>
#include "macros.h"

namespace Log
{
    std::map<std::string, logger_level> Logger::filtered = {{"ApplicationManager", logger_level::LOGINFO},
                                                            {"CellularMux", logger_level::LOGINFO},
                                                            {"ServiceCellular", logger_level::LOGINFO},
                                                            {"ServiceAntenna", logger_level::LOGINFO},
                                                            {"ServiceAudio", logger_level::LOGINFO},
                                                            {"ServiceBluetooth", logger_level::LOGINFO},
                                                            {"ServiceFota", logger_level::LOGINFO},
                                                            {"ServiceEink", logger_level::LOGINFO},
                                                            {"ServiceDB", logger_level::LOGINFO},
                                                            {"ServiceDesktop_w2", logger_level::LOGINFO},
                                                            {"SysMgrService", logger_level::LOGERROR},
                                                            {CRIT_STR, logger_level::LOGTRACE},
                                                            {IRQ_STR, logger_level::LOGTRACE}};
    const char *Logger::levelNames[]                     = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    Logger::Logger() : circularBuffer(circularBufferSize)
    {}

    const char *Logger::getTaskDesc()
    {
        return xTaskGetCurrentTaskHandle() == nullptr
                   ? Log::Logger::CRIT_STR
                   : xPortIsInsideInterrupt() ? Log::Logger::IRQ_STR : pcTaskGetName(xTaskGetCurrentTaskHandle());
    }

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
        LockGuard lock(mutex);
        if (auto it = filtered.find(name); it != std::end(filtered)) {
            return it->second;
        }
        filtered[name] = logger_level::LOGTRACE;
        return filtered[name];
    }

    bool Logger::setLogLevel(const std::string &name, logger_level level)
    {
        LockGuard lock(mutex);
        filtered[name] = level;
        return true;
    }

    bool Logger::filterLogs(logger_level level)
    {
        return getLogLevel(getTaskDesc()) <= level;
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
