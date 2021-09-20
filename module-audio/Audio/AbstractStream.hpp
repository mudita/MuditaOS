// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioFormat.hpp"

#include <cstdint>

namespace audio
{
    /**
     * @brief An abstract interface for classes implementing data connection
     * between two endpoints. The data is stored in blocks and accessed just
     * like in a regular FIFO. It provides an API to implement a zero-copy
     * mechanism.
     *
     */
    class AbstractStream
    {
      public:
        /**
         * @brief Events that are broadcasted to listeners by an AbstractStream
         *
         */
        enum class Event
        {
            NoEvent,
            StreamFull,
            StreamHalfUsed,
            StreamEmpty,
            StreamOverflow,
            StreamUnderFlow
        };

        /**
         * @brief An abstract interface for AbstractStream's event listeners
         *
         */
        class EventListener
        {
          public:
            /**
             * @brief Called on event by AbstractStream
             *
             * @param stream - stream reporting the event
             * @param event - type of event
             */
            virtual void onEvent(AbstractStream *stream, Event event) = 0;
            virtual ~EventListener()                                  = default;
        };

        /**
         * @brief A structure describing a range of raw data using a pair of
         * pointer and the size of data.
         */
        struct Span
        {
            /**
             * @brief Pointer to raw data.
             */
            std::uint8_t *data = nullptr;

            /**
             * @brief Size of data
             */
            std::size_t dataSize = 0;

            /**
             * @brief Calculate pointer to the end of the raw buffer
             *
             * @return pointer to the end of data.
             */
            constexpr std::uint8_t *dataEnd() const noexcept
            {
                return data + dataSize;
            }

            /**
             * @brief Resets the stored value to Null
             *
             */
            constexpr void reset() noexcept
            {
                data     = nullptr;
                dataSize = 0;
            }

            inline bool operator==(const Span &other) const noexcept
            {
                return data == other.data && dataSize == other.dataSize;
            }

            inline bool operator!=(const Span &other) const noexcept
            {
                return !operator==(other);
            }
        };

        /**
         * @brief A structure describing characteristics of an AbstractStream's
         * endpoint.
         *
         */
        struct Traits
        {
            /**
             * @brief the size of data to read/write in a single operation
             */
            std::size_t blockSize = 0;

            /**
             * @brief the format of data stored in the AbstractStream
             */
            AudioFormat format = nullFormat;
        };

        virtual ~AbstractStream() = default;

        /**
         * @brief Registers events listener
         *
         * @param listener to register
         */
        virtual void registerListener(EventListener *listener) = 0;

        /**
         * @brief Unregisters events listener
         *
         * @param listener to unregister
         */
        virtual void unregisterListeners(EventListener *listener) = 0;

        /**
         * @brief Fills single block with data provided
         *
         * @param data - a pointer to raw data to be copied from
         * @param dataSize - size of the data
         * @return true if operation succeded, false otherwise
         * @return false
         */
        virtual bool push(void *data, std::size_t dataSize) = 0;

        /**
         * @brief Fills single block with data provided
         *
         * @param span describing block of data to be copied
         *
         * @return true if operation succeded, false otherwise
         */
        virtual bool push(const Span &span) = 0;

        /**
         * @brief Fills single block with empty data
         *
         * @return true if operation succeded, false otherwise
         */
        virtual bool push() = 0;

        /**
         * @brief Copies one block of data to a buffer provided as an argument
         *
         * @param span - a place to copy stream's data to
         *
         * @return true if operation succeded, false otherwise
         */
        virtual bool pop(Span &span) = 0;

        /**
         * @brief Reserves a block in a stream for writing
         *
         * @param span - a space reserved within the stream
         * @return true if operation succeded, false otherwise
         */
        virtual bool reserve(Span &span) = 0;

        /**
         * @brief Marks reserved block as ready and writes it to the stream.
         */
        virtual void commit() = 0;

        /**
         * @brief Discards data written to the reserved block
         */
        virtual void release() = 0;

        /**
         * @brief Gets pointer and size of data ready to read without copying.
         *
         * @param span
         *
         * @return true if operation succeded, false otherwise
         */
        virtual bool peek(Span &span) = 0;

        /**
         * @brief Marks peeked data as read.
         */
        virtual void consume() = 0;

        /**
         * @brief Reset peek state without consuming it.
         */
        virtual void unpeek() = 0;

        /**
         * @brief Resets the state of a stream; discards all data
         */
        virtual void reset() = 0;

        /**
         * @brief Get the traits of the stream's input.
         *
         * @return Traits
         */
        [[nodiscard]] virtual auto getInputTraits() const noexcept -> Traits = 0;

        /**
         * @brief Get the traits of the stream's output.
         *
         * @return Traits
         */
        [[nodiscard]] virtual auto getOutputTraits() const noexcept -> Traits = 0;

        /**
         * @brief Checks if stream is empty.
         */
        [[nodiscard]] virtual bool isEmpty() const noexcept = 0;

        /**
         * @brief Checks if stream is full.
         */
        [[nodiscard]] virtual bool isFull() const noexcept = 0;
    };

}; // namespace audio
