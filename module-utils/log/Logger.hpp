// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <assert.h>
#include <board.h>
#include "log.hpp"
#include "log_colors.hpp"
#include <map>
#include <mutex.hpp>
#include <string>

namespace Log
{
    enum class Device
    {
        DEFAULT,
        SEGGER_RTT
    };

    class Logger
    {
      public:
        void enableColors(bool enable);
        [[nodiscard]] static Logger &get()
        {
            static Logger logger;
            return logger;
        }
        void init();
        auto log(Device device, const char *fmt, va_list args) -> int;
        void log(logger_level level, const char *file, int line, const char *function, const char *fmt, va_list args);
        auto logAssert(const char *fmt, va_list args) -> int;

        static constexpr auto CRIT_STR = "CRIT";
        static constexpr auto IRQ_STR  = "IRQ";

      private:
        Logger() = default;

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
        [[nodiscard]] auto GetLogLevel(const std::string &name) -> logger_level;
        bool lock();
        void logToDevice(const char *fmt, va_list args);
        void logToDevice(Device device, std::string_view log, size_t length);
        [[nodiscard]] size_t loggerBufferSizeLeft() const noexcept
        {
            const auto sizeLeft = LOGGER_BUFFER_SIZE - loggerBufferCurrentPos;
            assert(sizeLeft > 0);
            return sizeLeft;
        }
        void unlock();

        BaseType_t bt;
        cpp_freertos::MutexStandard mutex;
        logger_level level{LOGTRACE};
        const LogColors *logColors            = &logColorsOff;
        char loggerBuffer[LOGGER_BUFFER_SIZE] = {0};
        size_t loggerBufferCurrentPos         = 0;

        static const char *level_names[];
        static std::map<std::string, logger_level> filtered;
    };
} // namespace Log
