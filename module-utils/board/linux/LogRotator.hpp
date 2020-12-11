// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>
#include <thread.hpp>

namespace Log
{
    class LogRotator : public cpp_freertos::Thread
    {
        std::filesystem::path _logDir;
        const std::string _baseLogFileName;
        const size_t _maxLogFileSize;
        const unsigned _maxLogFilesCount;
        size_t _currentLogFileSize{0};
        unsigned _currentLogFileIndex{0};

        std::string _log;
        size_t _length;
        std::mutex _mutex;
        std::condition_variable _cond;
        std::atomic_bool _logFlag{false};

        void CreateLogDir();
        void RotateLogFile();
        void Run() override;
        void SaveToFile();

      public:
        LogRotator(std::string_view baseLogFileName, size_t maxLogFileSize, unsigned maxLogFilesCount);
        void Log(std::string_view log, size_t length);
    };
} // namespace Log
