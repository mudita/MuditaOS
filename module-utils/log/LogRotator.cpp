// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LogRotator.hpp"
#include <fstream>
#include <future>
#include <vfs.hpp>
#include <task.h>
#include <FreeRTOSConfig.h>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace Log
{
    LogRotator::LogRotator(std::string_view baseLogFileName, size_t maxLogFileSize, unsigned maxLogFilesCount)
        : Thread(configMINIMAL_STACK_SIZE, tskIDLE_PRIORITY), _baseLogFileName(baseLogFileName),
          _maxLogFileSize(maxLogFileSize), _maxLogFilesCount(maxLogFilesCount)
    {
        Start();
    }

    void LogRotator::Log(std::string_view log, size_t length)
    {
        std::unique_lock<std::mutex> lk(m);
        _log.append(log, 0, length);
        logFlag.store(true);
        lk.unlock();
        cv.notify_one();
    }

    void LogRotator::RotateLogFile()
    {
        ++_currentLogFileIndex;
        if (_currentLogFileIndex >= _maxLogFilesCount) {
            _currentLogFileIndex = 0;
        }
        _currentLogFileSize = 0;
    }

    void LogRotator::Run()
    {
        while (!vfs.isInitialized()) {}
        while (true) {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [this] { return logFlag.load(); });
            const std::fstream::openmode mode =
                (_currentLogFileSize == 0) ? std::fstream::out : std::fstream::out | std::fstream::app;
            std::fstream logFile(_baseLogFileName + std::to_string(_currentLogFileIndex) + ".log", mode);
            std::cout << "Dumping _log to file:\n" << _log << std::endl;
            logFile.write(_log.data(), _log.size());
            _currentLogFileSize += _length;
            if (_currentLogFileSize >= _maxLogFileSize) {
                RotateLogFile();
            }
            logFlag.store(false);
            _log.clear();
        }
    }

} // namespace Log
