// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MonoToStereo.hpp"

#include <Audio/AudioFormat.hpp>

using audio::transcode::MonoToStereo;

auto MonoToStereo::transform(const Span &span, const Span &transformSpace) const -> Span
{
    auto outputSpan   = Span{.data = transformSpace.data, .dataSize = transformBlockSize(span.dataSize)};
    auto outputBuffer = reinterpret_cast<std::uint16_t *>(transformSpace.data);
    auto inputBuffer  = reinterpret_cast<std::uint16_t *>(span.data);

    for (std::size_t i = span.dataSize / sizeof(std::uint16_t); i > 0; i--) {
        outputBuffer[i * 2 - 1] = outputBuffer[i * 2 - 2] = inputBuffer[i - 1];
    }

    return outputSpan;
}

auto MonoToStereo::transformBlockSize(std::size_t inputBufferSize) const noexcept -> std::size_t
{
    return 2 * inputBufferSize;
}

auto MonoToStereo::transformBlockSizeInverted(std::size_t outputBufferSize) const noexcept -> std::size_t
{
    return outputBufferSize / 2;
}

auto MonoToStereo::validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool
{
    return inputFormat.getChannels() == 1;
}

auto MonoToStereo::transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat
{
    return audio::AudioFormat(inputFormat.getSampleRate(), inputFormat.getBitWidth(), 2);
}
