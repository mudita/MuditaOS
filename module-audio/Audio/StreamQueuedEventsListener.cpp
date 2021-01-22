// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StreamQueuedEventsListener.hpp"

using namespace audio;

StreamQueuedEventsListener::StreamQueuedEventsListener(std::shared_ptr<cpp_freertos::Queue> eventsQueue)
    : queue(eventsQueue)
{}

void StreamQueuedEventsListener::onEvent(Stream *stream, Stream::Event event, Stream::EventSourceMode source)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    EventStorage newStorage                = {stream, event};

    if (source == Stream::EventSourceMode::ISR) {
        queue->EnqueueFromISR(&newStorage, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken) {
            taskYIELD();
        }
    }
    else if (!queue->Enqueue(&newStorage)) {
        LOG_ERROR("Queue full.");
    }
}

StreamQueuedEventsListener::queuedEvent StreamQueuedEventsListener::waitForEvent()
{
    EventStorage queueStorage;
    if (queue->Dequeue(&queueStorage)) {
        return std::make_pair(queueStorage.stream, queueStorage.event);
    }
    return std::make_pair(nullptr, Stream::Event::NoEvent);
}

std::size_t StreamQueuedEventsListener::getEventsCount() const
{
    return queue->NumItems();
}

StreamQueuedEventsListener::queuedEvent StreamQueuedEventsListener::getEvent()
{
    EventStorage queueStorage;

    if (queue->Dequeue(&queueStorage, 0)) {
        return std::make_pair(queueStorage.stream, queueStorage.event);
    }

    return std::make_pair(nullptr, Stream::Event::NoEvent);
}
