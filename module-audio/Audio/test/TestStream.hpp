// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AbstractStream.hpp>

#include <memory>

namespace testing::audio
{

    class TestStream : public ::audio::AbstractStream
    {
      public:
        explicit TestStream(std::size_t blockSize);

        // listener registration
        void registerListener(EventListener *listener) override;
        void unregisterListeners(EventListener *listener) override;

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

        [[nodiscard]] auto getInputTraits() const noexcept -> Traits override;
        [[nodiscard]] auto getOutputTraits() const noexcept -> Traits override;
        [[nodiscard]] bool isEmpty() const noexcept override;
        [[nodiscard]] bool isFull() const noexcept override;

        const Span getDataSpan() const;

        void setData(uint8_t value);
        bool checkData(const Span &span);

      private:
        std::unique_ptr<std::uint8_t[]> data;
        std::size_t bufSize;
    };

}; // namespace testing::audio
