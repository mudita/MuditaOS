// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <string_view>
#include <thread.hpp>
#include <condition_variable.hpp>
#include <mutex.hpp>

namespace Log
{
    class LogRotator : public cpp_freertos::Thread
    {
        const std::string _baseLogFileName;
        const size_t _maxLogFileSize;
        const unsigned _maxLogFilesCount;
        size_t _currentLogFileSize{0};
        unsigned _currentLogFileIndex{0};

        std::string _log;
        size_t _length;
        cpp_freertos::MutexStandard _mutex;
        cpp_freertos::ConditionVariable _cv;

        void RotateLogFile();
        void Run() override;
        void SaveToFile(std::string filePath);

      public:
        LogRotator(std::string_view baseLogFileName, size_t maxLogFileSize, unsigned maxLogFilesCount);
        void Log(std::string_view log, size_t length);
        void StartThread();
    };
} // namespace Log
