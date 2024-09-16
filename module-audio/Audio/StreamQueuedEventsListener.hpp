// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
            AbstractStream *stream      = nullptr;
            AbstractStream::Event event = AbstractStream::Event::NoEvent;
        };

      public:
        using QueuedEvent                         = std::pair<AbstractStream *, AbstractStream::Event>;
        static constexpr auto listenerElementSize = sizeof(EventStorage);

        explicit StreamQueuedEventsListener(std::shared_ptr<cpp_freertos::Queue> eventsQueue);
        virtual ~StreamQueuedEventsListener() = default;

        void onEvent(AbstractStream *stream, Stream::Event event) override;

        QueuedEvent waitForEvent();
        QueuedEvent getEvent();

        std::size_t getEventsCount() const;

      private:
        std::shared_ptr<cpp_freertos::Queue> queue;
    };
}; // namespace audio
