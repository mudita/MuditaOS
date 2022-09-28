// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Transform.hpp"

#include <integer.hpp>

#include <type_traits>

#include <cstdint>

namespace audio::transcode
{
    /**
     * @brief Basic decimation transformation - for every Ratio samples it drops
     * Ratio - 1 samples. The transformation is performed using basic integer type
     * to allow compiler to perform loop optimizations. The transformation is performed
     * in-place.
     *
     * @tparam SampleType - type of a single PCM sample, e.g., std::uint16_t for LPCM16
     * @tparam Channels - number of channels; 1 for mono, 2 for stereo
     * @tparam Ratio - order of the decimator; e.g.: for Ratio = 4 drops 3 sample for each block of 4
     * reducing sample rate by the factor of 4.
     */
    template <typename SampleType, unsigned int Channels, unsigned int Ratio>
    class BasicDecimator : public Transform
    {
        static_assert(Channels == 1 || Channels == 2);
        static_assert(std::is_integral<SampleType>::value);
        static_assert(Ratio > 0);

        /**
         * @brief Integer type to be used to read and write data from/to a buffer.
         */
        using IntegerType =
            typename decltype(utils::integer::getIntegerType<sizeof(SampleType) * utils::integer::BitsInByte *
                                                             Channels>())::type;

      public:
        auto transformBlockSize(std::size_t blockSize) const noexcept -> std::size_t override
        {
            return blockSize / Ratio;
        }

        auto transformBlockSizeInverted(std::size_t blockSize) const noexcept -> std::size_t override
        {
            return blockSize * Ratio;
        }

        auto transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat override
        {
            return audio::AudioFormat{
                inputFormat.getSampleRate() / Ratio, inputFormat.getBitWidth(), inputFormat.getChannels()};
        }

        auto validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool override
        {
            return sizeof(SampleType) * utils::integer::BitsInByte == inputFormat.getBitWidth();
        }

        auto transform(const Span &inputSpan, const Span &transformSpace) const -> Span override
        {
            auto outputSpan     = Span{.data = transformSpace.data, .dataSize = transformBlockSize(inputSpan.dataSize)};
            IntegerType *input  = reinterpret_cast<IntegerType *>(inputSpan.data);
            IntegerType *output = reinterpret_cast<IntegerType *>(outputSpan.data);

            for (unsigned i = 0; i < inputSpan.dataSize / sizeof(IntegerType) / Ratio; i++) {
                output[i] = input[i * Ratio];
            }

            return outputSpan;
        }
    };

} // namespace audio::transcode
