// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractStream.hpp"

#include <memory>

namespace audio
{

    /**
     * @brief Proxy for the AbstractStream class allowing to wrap its methods
     * with custom logic.
     */
    class StreamProxy : public AbstractStream
    {
      public:
        /**
         * @brief Construct a new Stream Proxy object
         *
         * @param wrappedStream - stream to wrap.
         */
        explicit StreamProxy(std::shared_ptr<AbstractStream> wrappedStream) noexcept;

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

      protected:
        auto getWrappedStream() -> AbstractStream &;

      private:
        std::shared_ptr<AbstractStream> wrappedStream;
    };

} // namespace audio
