// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory/NonCachedMemAllocator.hpp>
#include <CriticalSectionGuard.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <utility>

namespace audio
{
    class Stream
    {
      public:
        using UniqueStreamBuffer = std::unique_ptr<std::uint8_t[], std::function<void(uint8_t[])>>;

        struct Span
        {
            std::uint8_t *data   = nullptr;
            std::size_t dataSize = 0;

            std::uint8_t *dataEnd() const noexcept;
        };

        class RawBlockIterator
        {
          public:
            RawBlockIterator(std::uint8_t *bufStart, std::size_t bufSize, std::uint8_t *ptr, std::size_t stepSize);

            bool operator==(const RawBlockIterator &rhs);
            bool operator!=(const RawBlockIterator &rhs);
            RawBlockIterator &operator++();
            RawBlockIterator &operator--();
            RawBlockIterator operator++(int);
            RawBlockIterator operator--(int);
            Span operator*();

          private:
            std::uint8_t *_bufStart = nullptr;
            std::uint8_t *_bufEnd   = nullptr;
            std::uint8_t *_curPos   = nullptr;
            std::size_t _stepSize   = 0;
        };

        class Allocator
        {
          public:
            virtual UniqueStreamBuffer allocate(std::size_t size) = 0;
        };

        enum class Event
        {
            NoEvent,
            StreamFull,
            StreamHalfUsed,
            StreamEmpty,
            StreamOverflow,
            StreamUnderFlow
        };

        enum class EventSourceMode
        {
            ISR,
            Thread
        };

        class EventListener
        {
          public:
            virtual void onEvent(Stream *stream, Event event, EventSourceMode source) = 0;
        };

        static constexpr auto defaultBufferingSize = 4U;

        Stream(Allocator &allocator, std::size_t blockSize, unsigned int bufferingSize = defaultBufferingSize);

        /// push
        bool push(void *data, std::size_t dataSize);
        bool push(const Span &span);
        bool push();

        /// pop
        bool pop(Span &span);

        /// zero copy write
        bool reserve(Span &span);
        void commit();
        void release();

        /// zero copy read
        bool peek(Span &span);
        void consume();
        void unpeek();

        /// get empty data span
        Span getNullSpan() const noexcept;

        void reset();

        [[nodiscard]] std::size_t getBlockSize() const noexcept;
        [[nodiscard]] std::size_t getBlockCount() const noexcept;
        [[nodiscard]] std::size_t getUsedBlockCount() const noexcept;
        [[nodiscard]] std::size_t getPeekedCount() const noexcept;
        [[nodiscard]] std::size_t getReservedCount() const noexcept;
        [[nodiscard]] bool isEmpty() const noexcept;
        [[nodiscard]] bool isFull() const noexcept;
        [[nodiscard]] bool blocksAvailable() const noexcept;

        void registerListener(EventListener *listener);
        void unregisterListeners(EventListener *listener);

      private:
        using LockGuard = cpp_freertos::CriticalSectionGuard;

        void broadcastEvent(Event event);
        void broadcastStateEvents();

        Allocator &_allocator;
        std::size_t _blockSize    = 0;
        std::size_t _blockCount   = 0;
        std::size_t _blocksUsed   = 0;
        std::size_t _peekCount    = 0;
        std::size_t _reserveCount = 0;
        UniqueStreamBuffer _buffer;
        UniqueStreamBuffer _emptyBuffer;
        std::list<EventListener *> listeners;

        RawBlockIterator _dataStart;
        RawBlockIterator _dataEnd;
        RawBlockIterator _peekPosition;
        RawBlockIterator _writeReservationPosition;
    };

    class StandardStreamAllocator : public Stream::Allocator
    {
      public:
        Stream::UniqueStreamBuffer allocate(std::size_t size);
    };

    class NonCacheableStreamAllocator : public Stream::Allocator
    {
      public:
        NonCacheableStreamAllocator() = default;
        Stream::UniqueStreamBuffer allocate(std::size_t size);

      private:
        NonCachedMemAllocator<uint8_t> allocator;
    };

} // namespace audio

namespace std
{
    template <> struct iterator_traits<audio::Stream::RawBlockIterator>
    {
        using iterator_category = std::forward_iterator_tag;
        using value_type        = audio::Stream::Span;
        using difference_type   = std::size_t;
        using pointer           = audio::Stream::Span *;
        using reference         = audio::Stream::Span &;
    };
}; // namespace std
