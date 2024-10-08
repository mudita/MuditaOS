// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
        using FileDeletedCallback = std::function<void()>;

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
                      const EndOfFileCallback &endOfFileCallback,
                      const FileDeletedCallback &fileDeletedCallback,
                      ChannelMode mode);
        ~DecoderWorker() override;

        auto init(std::list<sys::WorkerQueueInfo> queues = std::list<sys::WorkerQueueInfo>()) -> bool override;

        auto enablePlayback() -> bool;
        auto disablePlayback() -> bool;

      private:
        static constexpr std::size_t stackDepth = 12 * 1024;

        auto handleMessage(std::uint32_t queueID) -> bool override;
        void pushAudioData();
        bool stateChangeWait();

        using BufferInternalType = std::int16_t;

        static constexpr auto workerName            = "DecoderWorker";
        static constexpr auto workerPriority        = static_cast<UBaseType_t>(sys::ServicePriority::Idle);
        static constexpr auto listenerQueueName     = "DecoderWorkerQueue";
        static constexpr auto listenerQueueCapacity = 1024;

        AbstractStream *audioStreamOut = nullptr;
        Decoder *decoder               = nullptr;
        std::unique_ptr<StreamQueuedEventsListener> queueListener;
        bool playbackEnabled = false;
        cpp_freertos::BinarySemaphore stateSemaphore;

        const int bufferSize;
        std::unique_ptr<BufferInternalType[]> decoderBuffer;
        ChannelMode channelMode = ChannelMode::NoConversion;

        EndOfFileCallback endOfFileCallback;
        FileDeletedCallback fileDeletedCallback;
    };
} // namespace audio
