// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <string_view>
#include <thread.hpp>
#include <condition_variable>
#include <mutex>
#include <atomic>

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
        std::mutex m;
        std::condition_variable cv;
        std::atomic_bool logFlag{false};

        void RotateLogFile();
        void Run() override;

      public:
        LogRotator(std::string_view baseLogFileName, size_t maxLogFileSize, unsigned maxLogFilesCount);
        void Log(std::string_view log, size_t length);
    };
} // namespace Log
