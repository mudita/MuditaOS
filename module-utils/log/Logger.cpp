// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Logger.hpp"
#include "LockGuard.hpp"
#include <ticks.hpp>
#include <purefs/filesystem_paths.hpp>
#include <fstream>
#include <CrashdumpMetadataStore.hpp>
#include <gsl/util>

namespace
{
    inline constexpr int statusSuccess         = 1;
    inline constexpr auto streamBufferSize     = 64 * 1024;
    inline constexpr auto logFileNamePrefix    = "MuditaOS";
    inline constexpr auto logFileNameExtension = ".log";
    inline constexpr auto logFileNameSeparator = "_";
} // namespace

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

    Logger::Logger() : streamBuffer{std::make_unique<char[]>(streamBufferSize)}, rotator{logFileNameExtension}
    {
        filtered = {
            {"ApplicationManager", LoggerLevel::LOGINFO},
            {"CellularMux", LoggerLevel::LOGINFO},
#if (!LOG_SENSITIVE_DATA_ENABLED)
            {"ServiceCellular", LoggerLevel::LOGINFO},
#endif
            {"ServiceAntenna", LoggerLevel::LOGERROR},
            {"ServiceAudio", LoggerLevel::LOGINFO},
            {"ServiceBluetooth", LoggerLevel::LOGINFO},
            {"ServiceBluetooth_w1", LoggerLevel::LOGINFO},
            {"ServiceFota", LoggerLevel::LOGINFO},
            {"ServiceEink", LoggerLevel::LOGINFO},
            {"ServiceDB", LoggerLevel::LOGINFO},
            {CRIT_STR, LoggerLevel::LOGTRACE},
            {IRQ_STR, LoggerLevel::LOGTRACE},
            {"FileIndexer", LoggerLevel::LOGINFO},
            {"EventManager", LoggerLevel::LOGINFO}
        };

        const std::list<sys::WorkerQueueInfo> queueInfo{
            {LoggerWorker::SignalQueueName, LoggerWorker::SignalSize, LoggerWorker::SignalQueueLength}};
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
        if (_logger == nullptr) {
            _logger = new Logger;
        }
        return *_logger;
    }

    LoggerLevel Logger::getLogLevel(const std::string &name)
    {
        return filtered[name];
    }

    std::string Logger::getLogs()
    {
        LockGuard lock(mutex);

        std::string logs;
        while (!buffer.getFlushBuffer()->isEmpty()) {
            const auto msg = buffer.getFlushBuffer()->get();
            if (msg.has_value()) {
                logs += msg.value();
            }
        }
        return logs;
    }

    void Logger::init(Application app, std::size_t fileSize)
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

    int Logger::log(Device device, const char *fmt, va_list args)
    {
        LockGuard lock(mutex);
        lineBufferCurrentPos = 0;
        return writeLog(device, fmt, args);
    }

    int Logger::log(LoggerLevel level, const char *file, int line, const char *function, const char *fmt, va_list args)
    {
        if (!filterLogs(level)) {
            return -1;
        }
        LockGuard lock(mutex);
        lineBufferCurrentPos = 0;
        addLogHeader(level, file, line, function);
        return writeLog(Device::DEFAULT, fmt, args);
    }

    [[nodiscard]] std::size_t Logger::lineBufferSizeLeft() const noexcept
    {
        const auto sizeLeft = lineBufferSize - lineBufferCurrentPos;
        assert(sizeLeft > 0);
        return sizeLeft;
    }

    int Logger::writeLog([[maybe_unused]] Device device, const char *fmt, va_list args)
    {
        constexpr auto lineTerminationString = "\n";
        constexpr auto lineTerminationLength = 2; // '\n' + null-terminator

        const auto bufferSizeLeft = lineBufferSizeLeft();
        const auto bytesParsed    = vsnprintf(&lineBuffer[lineBufferCurrentPos], bufferSizeLeft, fmt, args);
        if (bytesParsed >= 0) {
            /* Leave space for line termination */
            lineBufferCurrentPos +=
                std::min(bufferSizeLeft - lineTerminationLength, static_cast<std::size_t>(bytesParsed));
            /* Terminate the line */
            lineBufferCurrentPos +=
                snprintf(&lineBuffer[lineBufferCurrentPos], lineBufferSizeLeft(), lineTerminationString);
            /* Write the line to device and to the buffer */
            logToDevice(Device::DEFAULT, lineBuffer, lineBufferCurrentPos);
            buffer.getCurrentBuffer()->put(std::string(lineBuffer, lineBufferCurrentPos));
            checkBufferState();
            return lineBufferCurrentPos;
        }
        return -1;
    }

    int Logger::logAssert(const char *fmt, va_list args)
    {
        LockGuard lock(mutex);
        logToDevice(fmt, args);
        return lineBufferCurrentPos;
    }

    /// @param logPath: file path to store the log
    /// @return: < 0 - error occured during log flush
    /// @return:   0 - log flush did not happen
    /// @return:   1 - log flush successflul
    int Logger::dumpToFile(const std::filesystem::path &logDirectoryPath, LoggerState loggerState)
    {
        if (preDumpToFile != nullptr) {
            preDumpToFile();
        }
        auto _ = gsl::finally([this] {
            if (postDumpToFile != nullptr) {
                postDumpToFile();
            }
        });

        if (logFileName.empty()) {
            const auto &osMetadata = Store::CrashdumpMetadata::getInstance().getMetadataString();
            logFileName = std::string(logFileNamePrefix) + logFileNameSeparator + osMetadata + logFileNameExtension;
        }

        const auto logFilePath = logDirectoryPath / logFileName;

        std::error_code errorCode;
        auto firstDump = !std::filesystem::exists(logFilePath, errorCode);

        if (errorCode) {
            if (loggerState == LoggerState::RUNNING) {
                LOG_ERROR("Failed to check if file '%s' exists, error: %d!", logFilePath.c_str(), errorCode.value());
            }
            return -EIO;
        }

        if (const auto maxSizeExceeded = (!firstDump && (std::filesystem::file_size(logFilePath) > maxFileSize));
            maxSizeExceeded) {
            if (loggerState == LoggerState::RUNNING) {
                LOG_DEBUG("Max log file size exceeded. Rotating log files...");
            }

            {
                LockGuard lock(logFileMutex);
                rotator.rotateFile(logFilePath);
            }
            firstDump = true;
        }

        {
            const auto &logs = getLogs();

            LockGuard lock(logFileMutex);
            std::ofstream logFile;

            /* In some implementations pubsetbuf has to be called before opening a stream to be effective */
            logFile.rdbuf()->pubsetbuf(streamBuffer.get(), streamBufferSize);

            logFile.open(logFilePath, std::fstream::out | std::fstream::app);
            if (!logFile.good()) {
                if (loggerState == LoggerState::RUNNING) {
                    LOG_ERROR("Failed to open log file '%s'!", logFilePath.c_str());
                }
                return -EIO;
            }

            if (firstDump) {
                addFileHeader(logFile);
            }
            logFile.write(logs.data(), logs.size());
            if (logFile.bad()) {
                if (loggerState == LoggerState::RUNNING) {
                    LOG_ERROR("Failed to flush logs to file '%s'!", logFilePath.c_str());
                }
                return -EIO;
            }
        }

        if (loggerState == LoggerState::RUNNING) {
            LOG_DEBUG("Flush to file '%s' ended successfully!", logFilePath.c_str());
        }

        return statusSuccess;
    }

    int Logger::diagnosticDump()
    {
        buffer.nextBuffer();
        worker->notify(LoggerWorker::Signal::DumpDiagnostic);
        writeLogsTimer.restart(writeLogsToFileInterval);
        return statusSuccess;
    }

    int Logger::flushLogs()
    {
        LOG_INFO("Shutdown dump logs");
        worker->close();
        writeLogsTimer.stop();
        buffer.nextBuffer();
        return dumpToFile(purefs::dir::getLogsPath(), LoggerState::STOPPED);
    }

    void Logger::checkBufferState()
    {
        const auto size = buffer.getCurrentBuffer()->getSize();

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
        if (xTaskGetCurrentTaskHandle() == nullptr) {
            return Log::Logger::CRIT_STR;
        }
        if (xPortIsInsideInterrupt() == pdTRUE) {
            return Log::Logger::IRQ_STR;
        }
        return pcTaskGetName(xTaskGetCurrentTaskHandle());
    }

    bool Logger::filterLogs(LoggerLevel level)
    {
        return getLogLevel(getTaskDesc()) <= level;
    }

    void Logger::addLogHeader(LoggerLevel level, const char *file, int line, const char *function)
    {
        auto bufferSizeLeft = lineBufferSizeLeft();
        auto bytesParsed    = snprintf(&lineBuffer[lineBufferCurrentPos],
                                    bufferSizeLeft,
                                    "%" PRIu32 " ms ",
                                    cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));
        if (bytesParsed >= 0) {
            lineBufferCurrentPos += std::min(bufferSizeLeft, static_cast<std::size_t>(bytesParsed));
        }

        bufferSizeLeft = lineBufferSizeLeft();
        bytesParsed    = snprintf(&lineBuffer[lineBufferCurrentPos],
                               bufferSizeLeft,
                               "%s%-5s %s[%s] %s%s:%d:%s:%s ",
                               logColors->levelColors[level].data(),
                               levelNames[level],
                               logColors->serviceNameColor.data(),
                               getTaskDesc(),
                               logColors->callerInfoColor.data(),
                               file,
                               line,
                               function,
                               logColors->resetColor.data());
        if (bytesParsed >= 0) {
            lineBufferCurrentPos += std::min(bufferSizeLeft, static_cast<std::size_t>(bytesParsed));
        }
    }

    std::size_t Logger::getMaxLineLength()
    {
        return lineBufferSize;
    }
} // namespace Log
