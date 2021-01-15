// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <fstream>
#include "LoggerBuffer.hpp"
#include <mutex.hpp>
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include <string_view>

namespace Log
{
    class LoggerWorker : public sys::Worker
    {
      public:
        LoggerWorker();

        [[nodiscard]] auto getQueue() const noexcept -> xQueueHandle
        {
            return Worker::getQueueHandleByName(loggerQueueName);
        }
        void handleLogMessage(std::string *logData);
        bool handleMessage(uint32_t queueID) override;

        static constexpr auto loggerQueueName = "loggerQueueBuffer";

      private:
        void dumpToFile();

        LoggerBuffer loggerBuffer;
        cpp_freertos::MutexStandard mutex;

        static constexpr auto logFileName        = "MuditaOS.log";
        static constexpr size_t loggerBufferSize = 100;
        static constexpr auto logFileSize        = 1000;
        static constexpr auto workerName         = "LoggerWorker";
        static constexpr auto workerPriority     = static_cast<UBaseType_t>(sys::ServicePriority::Idle);
    };
} // namespace Log
