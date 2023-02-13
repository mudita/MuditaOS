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
        void enableColors(bool enable);
        [[nodiscard]] static Logger &get();
        static void destroyInstance();
        auto getLogs() -> std::string;
        void init(Application app, size_t fileSize = MAX_LOG_FILE_SIZE);
        void createTimer(sys::Service *parent);
        auto log(Device device, const char *fmt, va_list args) -> int;
        auto log(logger_level level, const char *file, int line, const char *function, const char *fmt, va_list args)
            -> int;
        auto logAssert(const char *fmt, va_list args) -> int;
        auto dumpToFile(std::filesystem::path logPath, bool isLoggerRunning = true) -> int;
        auto diagnosticDump() -> int;
        auto flushLogs() -> int;

        static constexpr auto CRIT_STR = "CRIT";
        static constexpr auto IRQ_STR  = "IRQ";

      private:
        Logger();

        void addLogHeader(logger_level level,
                          const char *file     = nullptr,
                          int line             = -1,
                          const char *function = nullptr);
        [[nodiscard]] bool filterLogs(logger_level level);
        /// Filter out not interesting logs via thread Name
        /// its' using fact that:
        /// - TRACE is level 0, for unedfined lookups it will be alvways trace
        /// - it will be one time init for apps which doesn't tell what level they should have
        /// - for others it will be o1 lookup so it's fine
        [[nodiscard]] auto getLogLevel(const std::string &name) -> logger_level;
        void logToDevice(const char *fmt, va_list args);
        void logToDevice(Device device, std::string_view logMsg, size_t length);
        auto writeLog(Device device, const char *fmt, va_list args) -> int;
        [[nodiscard]] size_t loggerBufferSizeLeft() const noexcept
        {
            const auto sizeLeft = LOGGER_BUFFER_SIZE - loggerBufferCurrentPos;
            assert(sizeLeft > 0);
            return sizeLeft;
        }

        void addFileHeader(std::ofstream &file) const;
        void checkBufferState();

        cpp_freertos::MutexStandard mutex;
        cpp_freertos::MutexStandard logFileMutex;
        logger_level level{LOGTRACE};
        const LogColors *logColors            = &logColorsOff;
        char loggerBuffer[LOGGER_BUFFER_SIZE] = {0};
        size_t loggerBufferCurrentPos         = 0;
        size_t maxFileSize                    = MAX_LOG_FILE_SIZE;

        Application application;

        LoggerBufferContainer buffer;

        utils::Rotator<MAX_LOG_FILES_COUNT> rotator;

        sys::TimerHandle writeLogsTimer;

        static const char *levelNames[];
        std::map<std::string, logger_level> filtered;
        static Logger *_logger;
        std::unique_ptr<LoggerWorker> worker;
    };

    const char *getTaskDesc();
} // namespace Log
