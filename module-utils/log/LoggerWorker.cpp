// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LoggerWorker.hpp"
#include "Logger.hpp"
#include <log/log.hpp>
#include <magic_enum.hpp>
#include <purefs/filesystem_paths.hpp>

namespace Log
{
    LoggerWorker::LoggerWorker(const std::string name) : Worker(name, priority)
    {}

    void LoggerWorker::notify(Signal command)
    {
        if (auto queue = getQueueByName(SignalQueueName); !queue->Overwrite(&command)) {
            LOG_ERROR("Unable to overwrite the command in the commands queue.");
        }
    }

    bool LoggerWorker::handleMessage(std::uint32_t queueID)
    {
        if (const auto queue = queues[queueID]; queue->GetQueueName() == SignalQueueName) {
            if (sys::WorkerCommand command; queue->Dequeue(&command, 0)) {
                handleCommand(static_cast<Signal>(command.command));
            }
        }
        return true;
    }

    void LoggerWorker::handleCommand(Signal command)
    {
        switch (command) {
        case Signal::DumpFilledBuffer:
        case Signal::DumpIntervalBuffer:
        case Signal::DumpDiagnostic:
            LOG_INFO("Received signal: %s", magic_enum::enum_name(command).data());
            Log::Logger::get().dumpToFile(purefs::dir::getLogsPath() / LOG_FILE_NAME);
            break;
        default:
            LOG_ERROR("Command not valid: %d", static_cast<int>(command));
        }
    }

} // namespace Log
