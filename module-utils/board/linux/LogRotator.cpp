// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LogRotator.hpp"
#include <fstream>
#include <pwd.h>
#include <vfs.hpp>

namespace Log
{
    LogRotator::LogRotator(std::string_view baseLogFileName, size_t maxLogFileSize, unsigned maxLogFilesCount)
        : Thread(configMINIMAL_STACK_SIZE, tskIDLE_PRIORITY), _baseLogFileName(baseLogFileName),
          _maxLogFileSize(maxLogFileSize), _maxLogFilesCount(maxLogFilesCount)
    {
        CreateLogDir();
        Start();
    }

    void LogRotator::CreateLogDir()
    {
        const char *homeDir;
        if ((homeDir = getenv("HOME")) == NULL) {
            homeDir = getpwuid(getuid())->pw_dir;
        }
        _logDir = homeDir;
        _logDir += "/log";

        const std::string mkdirCmd = "mkdir -p " + _logDir.string();
        system(mkdirCmd.c_str());
    }

    void LogRotator::Log(std::string_view log, size_t length)
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _log.append(log, 0, length);
        _logFlag.store(true);
        lk.unlock();
        _cond.notify_one();
    }

    void LogRotator::RotateLogFile()
    {
        ++_currentLogFileIndex;
        if (_currentLogFileIndex >= _maxLogFilesCount) {
            _currentLogFileIndex = 0;
        }
        _currentLogFileSize = 0;
    }

    void LogRotator::SaveToFile()
    {
        const std::fstream::openmode mode =
            (_currentLogFileSize == 0) ? std::fstream::out : std::fstream::out | std::fstream::app;
        const std::string logPath =
            _logDir.string() + "/" + LOF_FILE_NAME + std::to_string(_currentLogFileIndex) + ".log";
        std::fstream logFile(logPath, mode);
        logFile.write(_log.data(), _log.size());
    }

    void LogRotator::Run()
    {
        while (!vfs.isInitialized()) {}
        while (true) {
            std::unique_lock<std::mutex> lk(_mutex);
            _cond.wait(lk, [this] { return _logFlag.load(); });
            SaveToFile();
            _currentLogFileSize += _log.size();
            if (_currentLogFileSize >= _maxLogFileSize) {
                RotateLogFile();
            }
            _logFlag.store(false);
            _log.clear();
        }
    }

} // namespace Log
