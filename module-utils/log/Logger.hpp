// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <assert.h>
#include <board.h>
#include "log.hpp"
#include <map>
#ifdef TARGET_Linux
#include <mutex>
#else
#include <mutex.hpp>
#endif

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
        explicit Logger(logger_level level = LOGTRACE);
        auto log(Device device, const char *fmt, va_list args) -> int;
        void log(logger_level level, const char *file, int line, const char *function, const char *fmt, va_list args);
        auto logAssert(const char *fmt, va_list args) -> int;

        static constexpr auto CRIT_STR = "CRIT";
        static constexpr auto IRQ_STR  = "IRQ";

      private:
        void addLogHeader(logger_level level,
                          const char *file     = nullptr,
                          int line             = -1,
                          const char *function = nullptr);
        void dumpToFile();
        [[nodiscard]] bool filterLogs(logger_level level);
        /// Filter out not interesting logs via thread Name
        /// its' using fact that:
        /// - TRACE is level 0, for unedfined lookups it will be alvways trace
        /// - it will be one time init for apps which doesn't tell what level they should have
        /// - for others it will be o1 lookup so it's fine
        [[nodiscard]] auto filterThreadName(const std::string &name) -> logger_level;
        bool lock();
        void logToDevice(Device device, std::string_view log, size_t length);
        [[nodiscard]] size_t loggerBufferSizeLeft() const noexcept
        {
            const auto sizeLeft = LOGGER_BUFFER_SIZE - _loggerBufferCurrentPos;
            assert(sizeLeft > 0);
            return sizeLeft;
        }
        void unlock();

        std::map<std::string, logger_level> _filtered = {
            // {"ServiceDB", logger_level::LOGFATAL},
            {"ApplicationManager", logger_level::LOGINFO},
            {"TS0710Worker", logger_level::LOGINFO},
            {"ServiceCellular", logger_level::LOGINFO},
            {"ServiceAntenna", logger_level::LOGINFO},
            {"ServiceFota", logger_level::LOGINFO},
            {"ServiceEink", logger_level::LOGINFO},
            {"ServiceDB", logger_level::LOGINFO},
            // make sure that we got defined map entries for at least crit and irq
            // make sure that we got defined map entries for at least crit and irq
            {CRIT_STR, logger_level::LOGTRACE},
            {IRQ_STR, logger_level::LOGTRACE}};
#ifdef TARGET_Linux
        std::mutex _mutex;
#else
        BaseType_t _bt;
        cpp_freertos::MutexStandard _mutex;
#endif
        logger_level _level;
        char _loggerBuffer[LOGGER_BUFFER_SIZE] = {0};
        size_t _loggerBufferCurrentPos         = 0;
    };
} // namespace Log
