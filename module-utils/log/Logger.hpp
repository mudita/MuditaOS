// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "LoggerBuffer.hpp"
#include "LoggerWorker.hpp"
#include "LoggerBufferContainer.hpp"
#include "log_colors.hpp"
#include "Timers/TimerFactory.hpp"
#include <log/log.hpp>
#include <mutex.hpp>
#include <Service/Service.hpp>
#include <rotator/Rotator.hpp>

#include <assert.h>
#include <map>

namespace Log
{
    enum class Device
    {
        DEFAULT,
        SEGGER_RTT
    };

    enum class LoggerState
    {
        STOPPED,
        RUNNING
    };

    struct Application
    {
        std::string name;
        std::string revision;
        std::string tag;
        std::string branch;
    };
    std::ostream &operator<<(std::ostream &stream, const Application &application);

    class Logger
    {
      public:
        [[nodiscard]] static Logger &get();
        void enableColors(bool enable);
        static void destroyInstance();
        std::string getLogs();
        void init(Application app, std::size_t fileSize = defaultMaxLogFileSize);
        void createTimer(sys::Service *parent);
        int log(Device device, const char *fmt, va_list args);
        int log(LoggerLevel level, const char *file, int line, const char *function, const char *fmt, va_list args);
        int logAssert(const char *fmt, va_list args);
        int dumpToFile(const std::filesystem::path &logDirectoryPath, LoggerState loggerState = LoggerState::RUNNING);
        int diagnosticDump();
        int flushLogs();
        [[nodiscard]] std::size_t getMaxLineLength();

        /// functions called immediately before and after dumping logs to a file
        /// created to synchronize necessary peripherals with the CPU frequency
        std::function<void()> preDumpToFile;
        std::function<void()> postDumpToFile;

        static constexpr auto CRIT_STR = "CRIT";
        static constexpr auto IRQ_STR  = "IRQ";

      private:
        Logger();

        void addLogHeader(LoggerLevel level, const char *file = nullptr, int line = -1, const char *function = nullptr);
        [[nodiscard]] bool filterLogs(LoggerLevel level);
        /// Filter out not interesting logs via thread Name
        /// it's using fact that:
        /// - TRACE is level 0, for undefined lookups it will be always trace
        /// - it will be one time init for apps which doesn't tell what level they should have
        /// - for others it will be o1 lookup so it's fine
        [[nodiscard]] LoggerLevel getLogLevel(const std::string &name);
        void logToDevice(const char *fmt, va_list args);
        void logToDevice(Device device, const char *logMsg, std::size_t length);
        int writeLog(Device device, const char *fmt, va_list args);
        std::size_t lineBufferSizeLeft() const noexcept;

        void addFileHeader(std::ofstream &file) const;
        void checkBufferState();

        static constexpr std::size_t maxLogFilesCount      = 3;
        static constexpr std::size_t defaultMaxLogFileSize = 1024 * 1024 * 15; // 15 MB
        static constexpr std::size_t lineBufferSize        = 2048;

        cpp_freertos::MutexStandard mutex;
        cpp_freertos::MutexStandard logFileMutex;

        LoggerLevel loggerLevel{LOGTRACE};
        const LogColors *logColors            = &logColorsOff;
        static const char *levelNames[];
        std::map<std::string, LoggerLevel> filtered;

        char lineBuffer[lineBufferSize]  = {0};
        std::size_t lineBufferCurrentPos = 0;
        std::size_t maxFileSize          = defaultMaxLogFileSize;
        std::string logFileName;

        LoggerBufferContainer buffer;
        std::unique_ptr<char[]> streamBuffer;

        Application application;
        utils::Rotator<maxLogFilesCount> rotator;

        sys::TimerHandle writeLogsTimer;

        static Logger *_logger;
        std::unique_ptr<LoggerWorker> worker;
    };

    const char *getTaskDesc();
} // namespace Log
