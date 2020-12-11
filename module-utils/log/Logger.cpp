// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <fstream>
#include "Logger.hpp"
#ifdef TARGET_Linux
#include "board/linux/LogRotator.hpp"
#else
#include "board/cross/LogRotator.hpp"
#endif

namespace Log
{
    Logger::Logger(logger_level level) : _level{level}
    {}

    void Logger::dumpToFile()
    {
        if (_loggerBufferCurrentPos >= MAX_BUFFER_UTIL_MEM) {
            static LogRotator logRotator(LOF_FILE_NAME, MAX_LOG_FILE_SIZE, MAX_LOG_FILES_COUNT);
            logRotator.Log(_loggerBuffer, _loggerBufferCurrentPos);
            _loggerBufferCurrentPos = 0;
        }
    }

    auto Logger::filterThreadName(const std::string &name) -> logger_level
    {
        return _filtered[name];
    }

    auto Logger::log(Device device, const char *fmt, va_list args) -> int
    {
        if (!lock()) {
            return -1;
        }

        const size_t startingPos = _loggerBufferCurrentPos;
        _loggerBufferCurrentPos +=
            vsnprintf(&_loggerBuffer[_loggerBufferCurrentPos], loggerBufferSizeLeft(), fmt, args);
        const auto length = _loggerBufferCurrentPos - startingPos;
        logToDevice(device, &_loggerBuffer[startingPos], length);
        dumpToFile();

        unlock();
        return length;
    }

    void Logger::log(
        logger_level level, const char *file, int line, const char *function, const char *fmt, va_list args)
    {
        if (!filterLogs(level)) {
            return;
        }

        if (!lock()) {
            return;
        }

        const size_t startingPos = _loggerBufferCurrentPos;

        addLogHeader(level, file, line, function);

        _loggerBufferCurrentPos +=
            vsnprintf(&_loggerBuffer[_loggerBufferCurrentPos], loggerBufferSizeLeft(), fmt, args);
        _loggerBufferCurrentPos += snprintf(&_loggerBuffer[_loggerBufferCurrentPos], loggerBufferSizeLeft(), "\n");

        const auto length = _loggerBufferCurrentPos - startingPos;
        logToDevice(Device::DEFAULT, &_loggerBuffer[startingPos], length);
        dumpToFile();

        unlock();
    }

}; // namespace Log
