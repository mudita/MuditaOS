// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractStream.hpp"
#include "AudioFormat.hpp"

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
    class Stream : public AbstractStream
    {
      public:
        using UniqueStreamBuffer = std::unique_ptr<std::uint8_t[], std::function<void(uint8_t[])>>;
        using AbstractStream::Span;

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

        static constexpr auto defaultBufferingSize = 24U;

        Stream(AudioFormat format,
               Allocator &allocator,
               std::size_t blockSize,
               unsigned int bufferingSize = defaultBufferingSize);

        void registerListener(AbstractStream::EventListener *listener) override;
        void unregisterListeners(AbstractStream::EventListener *listener) override;

        /// push
        bool push(void *data, std::size_t dataSize) override;
        bool push(const Span &span) override;
        bool push() override;

        /// pop
        bool pop(Span &span) override;

        /// zero copy write
        bool reserve(Span &span) override;
        void commit() override;
        void release() override;

        /// zero copy read
        bool peek(Span &span) override;
        void consume() override;
        void unpeek() override;

        void reset() override;

        /// get empty data span
        Span getNullSpan() const noexcept;

        [[nodiscard]] auto getInputTraits() const noexcept -> Traits override;
        [[nodiscard]] auto getOutputTraits() const noexcept -> Traits override;
        [[nodiscard]] bool isEmpty() const noexcept override;
        [[nodiscard]] bool isFull() const noexcept override;

        [[nodiscard]] std::size_t getBlockCount() const noexcept;
        [[nodiscard]] std::size_t getUsedBlockCount() const noexcept;
        [[nodiscard]] std::size_t getPeekedCount() const noexcept;
        [[nodiscard]] std::size_t getReservedCount() const noexcept;
        [[nodiscard]] bool blocksAvailable() const noexcept;

      private:
        using LockGuard = cpp_freertos::CriticalSectionGuard;

        void broadcastEvent(Event event);
        void broadcastStateEvents();
        auto getIOTraits() const noexcept -> Traits;

        Allocator &_allocator;
        std::size_t _blockSize    = 0;
        std::size_t _blockCount   = 0;
        std::size_t _blocksUsed   = 0;
        std::size_t _peekCount    = 0;
        std::size_t _reserveCount = 0;
        AudioFormat _format       = nullFormat;
        UniqueStreamBuffer _buffer;
        UniqueStreamBuffer _emptyBuffer;
        std::list<AbstractStream::EventListener *> listeners;

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
    template <>
    struct iterator_traits<audio::Stream::RawBlockIterator>
    {
        using iterator_category = std::forward_iterator_tag;
        using value_type        = audio::Stream::Span;
        using difference_type   = std::size_t;
        using pointer           = audio::Stream::Span *;
        using reference         = audio::Stream::Span &;
    };
}; // namespace std
