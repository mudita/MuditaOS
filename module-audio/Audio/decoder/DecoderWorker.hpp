// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/StreamQueuedEventsListener.hpp>
#include <Audio/AbstractStream.hpp>

#include <Service/Worker.hpp>
#include <semaphore.hpp>

namespace audio
{
    class Decoder;
    class DecoderWorker : public sys::Worker
    {
      public:
        using EndOfFileCallback = std::function<void()>;
        enum class Command
        {
            EnablePlayback,
            DisablePlayback,
        };

        enum class ChannelMode
        {
            NoConversion,
            ForceStereo
        };

        DecoderWorker(AbstractStream *audioStreamOut,
                      Decoder *decoder,
                      EndOfFileCallback endOfFileCallback,
                      ChannelMode mode);
        ~DecoderWorker() override;

        virtual auto init(std::list<sys::WorkerQueueInfo> queues = std::list<sys::WorkerQueueInfo>()) -> bool override;

        auto enablePlayback() -> bool;
        auto disablePlayback() -> bool;

      private:
        static constexpr std::size_t stackDepth = 6 * 1024;

        virtual auto handleMessage(uint32_t queueID) -> bool override;
        void pushAudioData();
        bool stateChangeWait();

        using BufferInternalType = int16_t;

        static constexpr auto workerName            = "DecoderWorker";
        static constexpr auto workerPriority        = static_cast<UBaseType_t>(sys::ServicePriority::Idle);
        static constexpr auto listenerQueueName     = "DecoderWorkerQueue";
        static constexpr auto listenerQueueCapacity = 1024;

        AbstractStream *audioStreamOut = nullptr;
        Decoder *decoder               = nullptr;
        EndOfFileCallback endOfFileCallback;
        std::unique_ptr<StreamQueuedEventsListener> queueListener;
        bool playbackEnabled = false;
        cpp_freertos::BinarySemaphore stateSemaphore;

        const int bufferSize;
        std::unique_ptr<BufferInternalType[]> decoderBuffer;
        ChannelMode channelMode = ChannelMode::NoConversion;
    };
} // namespace audio
