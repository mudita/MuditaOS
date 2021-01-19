// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Stream.hpp"

#include <queue.hpp>

#include <cstdint>
#include <string>
#include <utility>

namespace audio
{
    class StreamQueuedEventsListener : public Stream::EventListener
    {
      private:
        struct EventStorage
        {
            Stream *stream      = nullptr;
            Stream::Event event = Stream::Event::NoEvent;
        };

      public:
        using queueInfo                           = std::pair<QueueHandle_t, std::string>;
        using queuedEvent                         = std::pair<Stream *, Stream::Event>;
        static constexpr auto listenerElementSize = sizeof(EventStorage);

        StreamQueuedEventsListener(std::shared_ptr<cpp_freertos::Queue> eventsQueue);

        void onEvent(Stream *stream, Stream::Event event, Stream::EventSourceMode source);

        queuedEvent waitForEvent();
        queuedEvent getEvent();

        std::size_t getEventsCount() const;

      private:
        std::shared_ptr<cpp_freertos::Queue> queue;
    };

}; // namespace audio
