// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gsl/gsl_util>
#include "LockGuard.hpp"
#include "LoggerWorker.hpp"
#include <module-vfs/include/user/purefs/filesystem_paths.hpp>

namespace Log
{
    LoggerWorker::LoggerWorker() : sys::Worker(workerName, workerPriority), loggerBuffer(loggerBufferSize)
    {}

    void LoggerWorker::handleLogMessage(std::string *logMsg)
    {
        LockGuard lock(mutex);

        loggerBuffer.put(std::move(*logMsg));
        if (loggerBuffer.getNumOfBytes() > logFileSize) {
            dumpToFile();
        }
    }

    bool LoggerWorker::handleMessage(uint32_t queueID)
    {
        auto queue          = queues[queueID]->GetQueueHandle();
        std::string *logMsg = nullptr;
        if (xQueueReceive(queue, &logMsg, 0) != pdTRUE) {
            return false;
        }
        handleLogMessage(logMsg);
        delete logMsg;
        return true;
    }

    void LoggerWorker::dumpToFile()
    {
        std::fstream logFile(purefs::dir::getUserDiskPath() / logFileName, std::fstream::out | std::fstream::app);
        while (!loggerBuffer.isEmpty()) {
            const auto &[_, logMsg] = loggerBuffer.get();
            logFile.write(logMsg.data(), logMsg.size());
        }
    }
} // namespace Log
