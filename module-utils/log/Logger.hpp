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
        void init(Application app, std::size_t fileSize = MAX_LOG_FILE_SIZE);
        void createTimer(sys::Service *parent);
        int log(Device device, const char *fmt, va_list args);
        int log(LoggerLevel level, const char *file, int line, const char *function, const char *fmt, va_list args);
        int logAssert(const char *fmt, va_list args);
        int dumpToFile(const std::filesystem::path &logPath, LoggerState loggerState = LoggerState::RUNNING);
        int diagnosticDump();
        int flushLogs();

        static constexpr auto CRIT_STR = "CRIT";
        static constexpr auto IRQ_STR  = "IRQ";

      private:
        Logger();

        void addLogHeader(LoggerLevel level, const char *file = nullptr, int line = -1, const char *function = nullptr);
        [[nodiscard]] bool filterLogs(LoggerLevel level);
        /// Filter out not interesting logs via thread Name
        /// its' using fact that:
        /// - TRACE is level 0, for unedfined lookups it will be alvways trace
        /// - it will be one time init for apps which doesn't tell what level they should have
        /// - for others it will be o1 lookup so it's fine
        [[nodiscard]] LoggerLevel getLogLevel(const std::string &name);
        void logToDevice(const char *fmt, va_list args);
        void logToDevice(Device device, const char *logMsg, std::size_t length);
        int writeLog(Device device, const char *fmt, va_list args);
        std::size_t lineBufferSizeLeft() const noexcept;

        void addFileHeader(std::ofstream &file) const;
        void checkBufferState();

        cpp_freertos::MutexStandard mutex;
        cpp_freertos::MutexStandard logFileMutex;
        LoggerLevel loggerLevel{LOGTRACE};
        const LogColors *logColors            = &logColorsOff;
        char lineBuffer[LINE_BUFFER_SIZE]     = {0};
        std::size_t lineBufferCurrentPos      = 0;
        std::size_t maxFileSize               = MAX_LOG_FILE_SIZE;

        Application application;

        LoggerBufferContainer buffer;

        utils::Rotator<MAX_LOG_FILES_COUNT> rotator;

        sys::TimerHandle writeLogsTimer;

        static const char *levelNames[];
        std::map<std::string, LoggerLevel> filtered;
        static Logger *_logger;
        std::unique_ptr<LoggerWorker> worker;

        static constexpr int statusSuccess = 1;

        static constexpr std::size_t streamBufferSize = 64 * 1024;
        std::unique_ptr<char[]> streamBuffer;
    };

    const char *getTaskDesc();
} // namespace Log
