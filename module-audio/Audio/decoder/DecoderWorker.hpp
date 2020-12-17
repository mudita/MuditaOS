// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Worker.hpp>
#include "Audio/StreamQueuedEventsListener.hpp"

namespace audio
{
    class Decoder;
    class DecoderWorker : public sys::Worker
    {
      public:
        using EndOfFileCallback = std::function<void()>;

        DecoderWorker(Stream &audioStreamOut, Decoder *decoder, EndOfFileCallback endOfFileCallback);
        ~DecoderWorker() override;

        virtual auto init(std::list<sys::WorkerQueueInfo> queues = std::list<sys::WorkerQueueInfo>()) -> bool override;
        virtual auto handleMessage(uint32_t queueID) -> bool override;

      private:
        using BufferInternalType = int16_t;

        static constexpr auto workerName            = "DecoderWorker";
        static constexpr auto workerPriority        = static_cast<UBaseType_t>(sys::ServicePriority::Idle);
        static constexpr auto listenerQueueName     = "DecoderWorkerQueue";
        static constexpr auto listenerQueueCapacity = 1024;

        Stream &audioStreamOut;
        Decoder *decoder = nullptr;
        EndOfFileCallback endOfFileCallback;
        std::unique_ptr<StreamQueuedEventsListener> queueListener;

        const int bufferSize;
        std::unique_ptr<BufferInternalType[]> decoderBuffer;
    };
} // namespace audio
