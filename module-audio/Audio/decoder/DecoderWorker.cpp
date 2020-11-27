// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DecoderWorker.hpp"
#include "Audio/decoder/Decoder.hpp"

audio::DecoderWorker::DecoderWorker(Stream &audioStreamOut, Decoder *decoder, EndOfFileCallback endOfFileCallback)
    : sys::Worker(DecoderWorker::workerName, DecoderWorker::workerPriority), audioStreamOut(audioStreamOut),
      decoder(decoder), endOfFileCallback(endOfFileCallback),
      bufferSize(audioStreamOut.getBlockSize() / sizeof(BufferInternalType))
{}

audio::DecoderWorker::~DecoderWorker()
{
    audioStreamOut.unregisterListeners(queueListener.get());
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

    audioStreamOut.registerListener(queueListener.get());

    decoderBuffer = std::make_unique<BufferInternalType[]>(bufferSize);
    if (!decoderBuffer) {
        return false;
    }

    return isSuccessful;
}

bool audio::DecoderWorker::handleMessage(uint32_t queueID)
{
    auto queue = queues[queueID];
    if (queue->GetQueueName() == listenerQueueName && queueListener) {
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
            auto samplesRead = 0;

            while (!audioStreamOut.isFull()) {
                samplesRead = decoder->decode(bufferSize, decoderBuffer.get());

                if (samplesRead == 0) {
                    endOfFileCallback();
                    break;
                }

                if (!audioStreamOut.push(decoderBuffer.get(), samplesRead * sizeof(BufferInternalType))) {
                    LOG_FATAL("Decoder failed to push to stream.");
                    break;
                }
            }
        }
    }
    return true;
}
