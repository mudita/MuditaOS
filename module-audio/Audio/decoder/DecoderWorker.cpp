// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DecoderWorker.hpp"
#include <Audio/AbstractStream.hpp>
#include <Audio/decoder/Decoder.hpp>

audio::DecoderWorker::DecoderWorker(audio::AbstractStream *audioStreamOut,
                                    Decoder *decoder,
                                    const EndOfFileCallback &endOfFileCallback,
                                    const FileDeletedCallback &fileDeletedCallback,
                                    ChannelMode mode)
    : sys::Worker(DecoderWorker::workerName, DecoderWorker::workerPriority, stackDepth), audioStreamOut(audioStreamOut),
      decoder(decoder), bufferSize(audioStreamOut->getInputTraits().blockSize / sizeof(BufferInternalType)),
      channelMode(mode), endOfFileCallback(endOfFileCallback), fileDeletedCallback(fileDeletedCallback)
{}

audio::DecoderWorker::~DecoderWorker()
{
    audioStreamOut->unregisterListeners(queueListener.get());
}

auto audio::DecoderWorker::init(std::list<sys::WorkerQueueInfo> queues) -> bool
{
    const std::list<sys::WorkerQueueInfo> list{
        {listenerQueueName, StreamQueuedEventsListener::listenerElementSize, listenerQueueCapacity}};

    const auto isSuccessful = Worker::init(list);

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

bool audio::DecoderWorker::handleMessage(std::uint32_t queueID)
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
    const auto readScale     = (channelMode == ChannelMode::ForceStereo) ? channel::stereoSound : channel::monoSound;
    std::int32_t samplesRead = 0;

    while (!audioStreamOut->isFull() && playbackEnabled) {
        auto buffer = decoderBuffer.get();
        const auto totalBufferSize = bufferSize / readScale;
        samplesRead                = decoder->decode(totalBufferSize, buffer);

        if (samplesRead == Decoder::fileDeletedRetCode) {
            fileDeletedCallback();
            break;
        }
        if (samplesRead < totalBufferSize) {
            endOfFileCallback();
            break;
        }

        // pcm mono to stereo force conversion
        if (channelMode == ChannelMode::ForceStereo) {
            for (auto i = bufferSize / 2; i > 0; i--) {
                buffer[i * 2 - 1] = buffer[i * 2 - 2] = buffer[i - 1];
            }
        }

        if (!audioStreamOut->push(decoderBuffer.get(), samplesRead * sizeof(BufferInternalType) * readScale)) {
            LOG_ERROR("Decoder failed to push to stream");
            break;
        }
    }
}

bool audio::DecoderWorker::enablePlayback()
{
    return sendCommand({.command = static_cast<std::uint32_t>(Command::EnablePlayback), .data = nullptr}) &&
           stateChangeWait();
}

bool audio::DecoderWorker::disablePlayback()
{
    return sendCommand({.command = static_cast<std::uint32_t>(Command::DisablePlayback), .data = nullptr}) &&
           stateChangeWait();
}

bool audio::DecoderWorker::stateChangeWait()
{
    return stateSemaphore.Take();
}
