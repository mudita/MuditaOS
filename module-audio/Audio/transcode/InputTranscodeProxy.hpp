// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/StreamProxy.hpp>

#include "Transform.hpp"

#include <memory>

namespace audio::transcode
{
    /**
     * @brief An input transcoding proxy for an AbstractStream.
     */
    class InputTranscodeProxy : public audio::StreamProxy
    {
      public:
        /**
         * @brief Construct a new Input Transcode Proxy object
         *
         * @param wrappedStream
         * @param transform to apply on the input
         */
        explicit InputTranscodeProxy(std::shared_ptr<AbstractStream> wrappedStream,
                                     std::shared_ptr<Transform> transform) noexcept;

        bool push(const Span &span) override;
        bool push(void *data, std::size_t dataSize) override;
        void commit() override;
        bool reserve(Span &span) override;
        [[nodiscard]] auto getInputTraits() const noexcept -> Traits override;

      private:
        std::shared_ptr<Transform> transform;
        Span reservedSpan;
        std::size_t transcodingSpaceSize;
        std::unique_ptr<std::uint8_t[]> transcodingSpace;
        Span transcodingSpaceSpan;
        bool isReserved = false;
    };

}; // namespace audio::transcode
