// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Logger.hpp"
#include "LockGuard.hpp"
#include <ticks.hpp>
#include <purefs/filesystem_paths.hpp>

#include <fstream>

namespace Log
{
    const char *Logger::levelNames[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    Logger *Logger::_logger          = nullptr;

    using namespace std::chrono_literals;
    constexpr std::chrono::minutes writeLogsToFileInterval = 15min;

    std::ostream &operator<<(std::ostream &stream, const Application &application)
    {
        stream << application.name << ' ' << application.revision << ", " << application.tag << ", "
               << application.branch << '\n';
        return stream;
    }

    Logger::Logger() : buffer{}, rotator{".log"}
    {
        filtered = {
            {"ApplicationManager", logger_level::LOGINFO},
            {"CellularMux", logger_level::LOGINFO},
#if (!LOG_SENSITIVE_DATA_ENABLED)
            {"ServiceCellular", logger_level::LOGINFO},
#endif
            {"ServiceAntenna", logger_level::LOGERROR},
            {"ServiceAudio", logger_level::LOGINFO},
            {"ServiceBluetooth", logger_level::LOGINFO},
            {"ServiceBluetooth_w1", logger_level::LOGINFO},
            {"ServiceFota", logger_level::LOGINFO},
            {"ServiceEink", logger_level::LOGINFO},
            {"ServiceDB", logger_level::LOGINFO},
            {CRIT_STR, logger_level::LOGTRACE},
            {IRQ_STR, logger_level::LOGTRACE},
            {"FileIndexer", logger_level::LOGINFO},
            {"EventManager", logger_level::LOGINFO}
        };

        std::list<sys::WorkerQueueInfo> queueInfo{
            {LoggerWorker::SignalQueueName, LoggerWorker::SignalSize, LoggerWorker::SignalQueueLenght}};
        worker = std::make_unique<LoggerWorker>(Log::workerName);
        worker->init(queueInfo);
        worker->run();
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

    void Logger::destroyInstance()
    {

        delete _logger;
        _logger = nullptr;
    }

    Logger &Logger::get()
    {
        static auto *logger = new Logger;
        _logger             = logger;
        return *logger;
    }

    auto Logger::getLogLevel(const std::string &name) -> logger_level
    {
        return filtered[name];
    }

    auto Logger::getLogs() -> std::string
    {
        LockGuard lock(mutex);

        std::string logs;
        while (!buffer.getFlushBuffer()->isEmpty()) {
            const auto [result, msg] = buffer.getFlushBuffer()->get();
            if (result) {
                logs += msg;
            }
        }
        return logs;
    }

    void Logger::init(Application app, size_t fileSize)
    {
        application = std::move(app);
        maxFileSize = fileSize;
#if LOG_USE_COLOR == 1
        enableColors(true);
#else
        enableColors(false);
#endif
    }

    void Logger::createTimer(sys::Service *parent)
    {
        writeLogsTimer = sys::TimerFactory::createPeriodicTimer(
            parent, "writeLogsToFileTimer", writeLogsToFileInterval, [this](sys::Timer &) {
                buffer.nextBuffer();
                worker->notify(LoggerWorker::Signal::DumpIntervalBuffer);
            });
        writeLogsTimer.start();
    }

    auto Logger::log(Device device, const char *fmt, va_list args) -> int
    {
        LockGuard lock(mutex);
        loggerBufferCurrentPos = 0;
        return writeLog(device, fmt, args);
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
        return writeLog(Device::DEFAULT, fmt, args);
    }

    auto Logger::writeLog(Device device, const char *fmt, va_list args) -> int
    {
        const auto sizeLeft = loggerBufferSizeLeft();
        const auto result   = vsnprintf(&loggerBuffer[loggerBufferCurrentPos], sizeLeft, fmt, args);
        if (0 <= result) {
            const auto numOfBytesAddedToBuffer = static_cast<size_t>(result);
            loggerBufferCurrentPos += (numOfBytesAddedToBuffer < sizeLeft) ? numOfBytesAddedToBuffer : (sizeLeft - 1);
            loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos], loggerBufferSizeLeft(), "\n");

            logToDevice(Device::DEFAULT, loggerBuffer, loggerBufferCurrentPos);
            buffer.getCurrentBuffer()->put(std::string(loggerBuffer, loggerBufferCurrentPos));
            checkBufferState();
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

    /// @param logPath: file path to store the log
    /// @return: < 0 - error occured during log flush
    /// @return:   0 - log flush did not happen
    /// @return:   1 - log flush successflul
    auto Logger::dumpToFile(std::filesystem::path logPath, bool isLoggerRunning) -> int
    {
        std::error_code errorCode;
        auto firstDump = !std::filesystem::exists(logPath, errorCode);

        if (errorCode) {
            if (isLoggerRunning) {
                LOG_ERROR("Failed to check if file %s exists, error: %d", logPath.c_str(), errorCode.value());
            }
            return -EIO;
        }

        if (const bool maxSizeExceeded = !firstDump && std::filesystem::file_size(logPath) > maxFileSize;
            maxSizeExceeded) {
            if (isLoggerRunning) {
                LOG_DEBUG("Max log file size exceeded. Rotating log files...");
            }

            {
                LockGuard lock(logFileMutex);
                rotator.rotateFile(logPath);
            }
            firstDump = true;
        }

        int status = 1;
        {
            const auto &logs = getLogs();

            LockGuard lock(logFileMutex);
            std::ofstream logFile(logPath, std::fstream::out | std::fstream::app);
            if (!logFile.good()) {
                status = -EIO;
            }

            constexpr size_t streamBufferSize = 64 * 1024;
            auto streamBuffer                 = std::make_unique<char[]>(streamBufferSize);
            logFile.rdbuf()->pubsetbuf(streamBuffer.get(), streamBufferSize);

            if (firstDump) {
                addFileHeader(logFile);
            }
            logFile.write(logs.data(), logs.size());
            if (logFile.bad()) {
                status = -EIO;
            }
        }

        if (isLoggerRunning) {
            LOG_DEBUG("Flush ended with status: %d", status);
        }

        return status;
    }

    auto Logger::diagnosticDump() -> int
    {
        buffer.nextBuffer();
        worker->notify(LoggerWorker::Signal::DumpDiagnostic);
        writeLogsTimer.restart(writeLogsToFileInterval);
        return 1;
    }

    auto Logger::flushLogs() -> int
    {
        LOG_INFO("Shutdown dump logs");
        worker->close();
        writeLogsTimer.stop();
        buffer.nextBuffer();
        return dumpToFile(purefs::dir::getLogsPath() / LOG_FILE_NAME, false);
    }

    void Logger::checkBufferState()
    {
        auto size = buffer.getCurrentBuffer()->getSize();

        if (size >= buffer.getCircularBufferSize()) {
            worker->notify(LoggerWorker::Signal::DumpFilledBuffer);
            buffer.nextBuffer();
            writeLogsTimer.restart(writeLogsToFileInterval);
        }
    }

    void Logger::addFileHeader(std::ofstream &file) const
    {
        file << application;
    }

    const char *getTaskDesc()
    {
        return xTaskGetCurrentTaskHandle() == nullptr ? Log::Logger::CRIT_STR
               : xPortIsInsideInterrupt()             ? Log::Logger::IRQ_STR
                                                      : pcTaskGetName(xTaskGetCurrentTaskHandle());
    }

    bool Logger::filterLogs(logger_level level)
    {
        return getLogLevel(getTaskDesc()) <= level;
    }

    void Logger::addLogHeader(logger_level level, const char *file, int line, const char *function)
    {
        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           LOGGER_BUFFER_SIZE - loggerBufferCurrentPos,
                                           "%" PRIu32 " ms ",
                                           cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));

        loggerBufferCurrentPos += snprintf(&loggerBuffer[loggerBufferCurrentPos],
                                           LOGGER_BUFFER_SIZE - loggerBufferCurrentPos,
                                           "%s%-5s %s[%s] %s%s:%s:%d:%s ",
                                           logColors->levelColors[level].data(),
                                           levelNames[level],
                                           logColors->serviceNameColor.data(),
                                           getTaskDesc(),
                                           logColors->callerInfoColor.data(),
                                           file,
                                           function,
                                           line,
                                           logColors->resetColor.data());
    }

} // namespace Log
