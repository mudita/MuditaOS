// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioFormat.hpp>
#include <Audio/AbstractStream.hpp>

namespace audio::transcode
{

    /**
     * @brief Data transform which can be applied on a data range.
     */
    class Transform
    {
      public:
        using Span = audio::AbstractStream::Span;

        virtual ~Transform() = default;

        /**
         * @brief Transforms data within range
         *
         * @param inputSpan - input data to transform
         * @param outputSpan - space for transformation output
         * @return Output data
         */
        virtual auto transform(const Span &inputSpan, const Span &outputSpan) const -> Span = 0;

        /**
         * @brief Checks if audio format is suitable for transformation.
         *
         * @param inputFormat - format to be checked
         * @return true if inputFormat is suitable for transformation, false otherwise
         */
        virtual auto validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool = 0;

        /**
         * @brief Calculates the audio format on transform output based on the
         * input format.
         *
         * @param inputFormat - format of the data on input
         * @return format of the data on output.
         */
        virtual auto transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat = 0;

        /**
         * @brief Calculates the size of an output data after the transform.
         *
         * @param blockSize - size of an input block
         * @return size of an output block
         */
        virtual auto transformBlockSize(std::size_t blockSize) const noexcept -> std::size_t = 0;

        /**
         * @brief Calculates required size of an input data to get transformed data of size equal
         * outputBlockSize
         *
         * @param outputBlockSize - size of an output block
         * @return std::size_t
         */
        virtual auto transformBlockSizeInverted(std::size_t outputBlockSize) const noexcept -> std::size_t = 0;

        /**
         * @brief A convenience transform operator.
         */
        inline auto operator()(const Span &span, const Span &transformSpace)
        {
            return transform(span, transformSpace);
        }

        /**
         * @brief A convenience wrapper for transforms which has transcoding size
         * same as output size and a transcoding operation is performed in-place.
         *
         * @param span to transform
         * @return transformed span
         */
        inline auto transform(const Span &span)
        {
            return transform(span, span);
        }
    };

}; // namespace audio::transcode
