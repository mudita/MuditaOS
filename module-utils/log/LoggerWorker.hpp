// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Worker.hpp>

namespace Log
{
    static constexpr auto workerName = "LoggerWorker";

    class LoggerWorker : public sys::Worker
    {
      public:
        enum class Signal
        {
            DumpFilledBuffer,
            DumpIntervalBuffer,
            DumpDiagnostic
        };

        static constexpr auto SignalQueueName   = "LoggerSignal";
        static constexpr auto SignalSize        = sizeof(Signal);
        static constexpr auto SignalQueueLenght = 1;

        explicit LoggerWorker(const std::string name);
        void notify(Signal command);
        bool handleMessage(std::uint32_t queueID) override;
        void handleCommand(Signal command);

      private:
        static constexpr auto priority = static_cast<UBaseType_t>(sys::ServicePriority::Idle);
    };

} // namespace Log
