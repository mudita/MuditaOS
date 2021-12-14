// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DecoderWorker.hpp"
#include <Audio/AbstractStream.hpp>
#include <Audio/decoder/Decoder.hpp>

audio::DecoderWorker::DecoderWorker(audio::AbstractStream *audioStreamOut,
                                    Decoder *decoder,
                                    EndOfFileCallback endOfFileCallback,
                                    ChannelMode mode)
    : sys::Worker(DecoderWorker::workerName, DecoderWorker::workerPriority, stackDepth), audioStreamOut(audioStreamOut),
      decoder(decoder), endOfFileCallback(endOfFileCallback),
      bufferSize(audioStreamOut->getInputTraits().blockSize / sizeof(BufferInternalType)), channelMode(mode)
{}

audio::DecoderWorker::~DecoderWorker()
{
    audioStreamOut->unregisterListeners(queueListener.get());
}

auto audio::DecoderWorker::init(std::list<sys::WorkerQueueInfo> queues) -> bool
{
    std::list<sys::WorkerQueueInfo> list{
        {listenerQueueName, StreamQueuedEventsListener::listenerElementSize, listenerQueueCapacity}};

    auto isSuccessful = Worker::init(list);

    queueListener = std::make_unique<StreamQueuedEventsListener>(getQueueByName(listenerQueueName));
    if (!queueListener) {
        return false;
    }

    audioStreamOut->registerListener(queueListener.get());

    decoderBuffer = std::make_unique<BufferInternalType[]>(bufferSize);
    if (!decoderBuffer) {
        return false;
    }

    return isSuccessful;
}

bool audio::DecoderWorker::handleMessage(uint32_t queueID)
{
    auto queue      = queues[queueID];
    auto &queueName = queue->GetQueueName();
    if (queueName == listenerQueueName && queueListener) {
        auto event = queueListener->getEvent();

        switch (event.second) {
        case Stream::Event::StreamOverflow:
            break;
        case Stream::Event::StreamUnderFlow:
            break;
        case Stream::Event::NoEvent:
            break;
        case Stream::Event::StreamFull:
            break;
        case Stream::Event::StreamHalfUsed:
            [[fallthrough]];
        case Stream::Event::StreamEmpty:
            pushAudioData();
        }
    }
    else if (queueName == SERVICE_QUEUE_NAME) {
        auto &serviceQueue = getServiceQueue();
        sys::WorkerCommand cmd;

        if (serviceQueue.Dequeue(&cmd)) {
            switch (static_cast<Command>(cmd.command)) {
            case Command::EnablePlayback: {
                playbackEnabled = true;
                stateSemaphore.Give();
                pushAudioData();
                break;
            }
            case Command::DisablePlayback: {
                playbackEnabled = false;
                stateSemaphore.Give();
            }
            }
        }
    }

    return true;
}

void audio::DecoderWorker::pushAudioData()
{
    auto samplesRead             = 0;
    const unsigned int readScale = channelMode == ChannelMode::ForceStereo ? 2 : 1;

    while (!audioStreamOut->isFull() && playbackEnabled) {
        auto buffer = decoderBuffer.get();
        samplesRead = decoder->decode(bufferSize / readScale, buffer);

        if (samplesRead == 0) {
            endOfFileCallback();
            break;
        }

        // pcm mono to stereo force conversion
        if (channelMode == ChannelMode::ForceStereo) {
            for (unsigned int i = bufferSize / 2; i > 0; i--) {
                buffer[i * 2 - 1] = buffer[i * 2 - 2] = buffer[i - 1];
            }
        }

        if (!audioStreamOut->push(decoderBuffer.get(), samplesRead * sizeof(BufferInternalType) * readScale)) {
            LOG_FATAL("Decoder failed to push to stream.");
            break;
        }
    }
}

bool audio::DecoderWorker::enablePlayback()
{
    return sendCommand({.command = static_cast<uint32_t>(Command::EnablePlayback), .data = nullptr}) &&
           stateChangeWait();
}

bool audio::DecoderWorker::disablePlayback()
{
    return sendCommand({.command = static_cast<uint32_t>(Command::DisablePlayback), .data = nullptr}) &&
           stateChangeWait();
}

bool audio::DecoderWorker::stateChangeWait()
{
    return stateSemaphore.Take();
}
