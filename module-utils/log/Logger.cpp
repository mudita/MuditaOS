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
                                                            {"ServiceFota", logger_level::LOGINFO},
                                                            {"ServiceEink", logger_level::LOGINFO},
                                                            {"ServiceDB", logger_level::LOGINFO},
                                                            {CRIT_STR, logger_level::LOGTRACE},
                                                            {IRQ_STR, logger_level::LOGTRACE}};
    const char *Logger::levelNames[]                     = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    void Logger::runWorker()
    {
        loggerWorker = std::make_unique<LoggerWorker>();
        const bool ret =
            loggerWorker->init({{LoggerWorker::loggerQueueName, sizeof(std::string *), loggerQueueLength}});
        if (ret == false) {
            constexpr std::string_view logMsg = "!!! Logger failed to initialize logger worker !!!";
            log(logMsg.data(), logMsg.size(), LOGERROR, __FILENAME__, __LINE__, __func__);
            return;
        }
        else {
            loggerWorker->run();
            loggerWorkerQueue = loggerWorker->getQueue();
        }
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

    auto Logger::GetLogLevel(const std::string &name) -> logger_level
    {
        return filtered[name];
    }

    auto Logger::log(const char *fmt, va_list args) -> int
    {
        return log(currentDevice, fmt, args);
    }

    void Logger::init()
    {
#if LOG_USE_COLOR == 1
        enableColors(true);
#else
        enableColors(false);
#endif
        runWorker();
    }

    auto Logger::log(Device device, const char *fmt, va_list args) -> int
    {
        LockGuard lock(mutex);

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
        LockGuard lock(mutex);

        loggerBufferCurrentPos = 0;
        addLogHeader(level, file, line, function);

        loggerBufferCurrentPos += vsnprintf(&loggerBuffer[loggerBufferCurrentPos], loggerBufferSizeLeft(), fmt, args);
        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos], loggerBufferSizeLeft(), "\n");

        logToDevice(currentDevice, loggerBuffer, loggerBufferCurrentPos);
    }

    void Logger::log(
        std::string_view logMsg, size_t length, logger_level level, const char *file, int line, const char *function)
    {
        loggerBufferCurrentPos = 0;
        addLogHeader(level, file, line, function);
        const auto size = std::min(length, loggerBufferSizeLeft());
        strncpy(&loggerBuffer[loggerBufferCurrentPos], logMsg.data(), size);
        logToDevice(Device::DEFAULT, loggerBuffer, size);
    }

    auto Logger::logAssert(const char *fmt, va_list args) -> int
    {
        LockGuard lock(mutex);
        logToDevice(fmt, args);
        return loggerBufferCurrentPos;
    }

    void Logger::sendLogMsgToWorker(std::string_view logMsg, size_t length)
    {
        if (loggerWorkerQueue == nullptr)
            return;
        if (uxQueueSpacesAvailable(loggerWorkerQueue) != 0) {
            auto data = new std::string(logMsg.data(), length);
            xQueueSend(loggerWorkerQueue, &data, portMAX_DELAY);
        }
        else {
            constexpr std::string_view logMsg = "!!! Logger worker queue is full !!!";
            log(logMsg.data(), logMsg.size(), LOGWARN, __FILENAME__, __LINE__, __func__);
        }
    }
}; // namespace Log
