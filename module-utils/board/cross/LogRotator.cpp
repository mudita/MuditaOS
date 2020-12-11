// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LogRotator.hpp"
#include <fstream>
#include <vfs.hpp>
#include <task.h>
#include <FreeRTOSConfig.h>

namespace Log
{
    LogRotator::LogRotator(std::string_view baseLogFileName, size_t maxLogFileSize, unsigned maxLogFilesCount)
        : Thread(configMINIMAL_STACK_SIZE, tskIDLE_PRIORITY), _baseLogFileName(baseLogFileName),
          _maxLogFileSize(maxLogFileSize), _maxLogFilesCount(maxLogFilesCount)
    {
        StartThread();
    }

    void LogRotator::Log(std::string_view log, size_t length)
    {
        const auto state = xTaskGetSchedulerState();
        if (state == taskSCHEDULER_RUNNING) {
            _mutex.Lock();
            _log.append(log, 0, length);
            _mutex.Unlock();
            _cv.Signal();
        }
        else {
            _log.append(log, 0, length);
        }
    }

    void LogRotator::StartThread()
    {
        Start();
    }

    void LogRotator::RotateLogFile()
    {
        ++_currentLogFileIndex;
        if (_currentLogFileIndex >= _maxLogFilesCount) {
            _currentLogFileIndex = 0;
        }
        _currentLogFileSize = 0;
    }

    void LogRotator::SaveToFile(std::string filePath)
    {
        const std::fstream::openmode mode =
            (_currentLogFileSize == 0) ? std::fstream::out : std::fstream::out | std::fstream::app;
        std::fstream logFile(filePath + std::to_string(_currentLogFileIndex) + ".log", mode);
        logFile.write(_log.data(), _log.size());
    }

    void LogRotator::Run()
    {
        while (!vfs.isInitialized()) {
            Delay(100);
        }

        while (true) {
            Delay(100);
            cpp_freertos::LockGuard lk(_mutex);

            while (_log.empty()) {
                Wait(_cv, _mutex);
                Delay(100);
            }
            // SaveToFile("/user/Mudita1OS");
            // SaveToFile("/home/Mudita2OS");
            SaveToFile("/current/Mudita3OS");
            _currentLogFileSize += _log.size();
            if (_currentLogFileSize >= _maxLogFileSize) {
                RotateLogFile();
            }
            _log.clear();
        }
    }

} // namespace Log
