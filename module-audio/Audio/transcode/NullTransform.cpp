// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NullTransform.hpp"

using ::audio::transcode::NullTransform;

auto NullTransform::transform(const Span &span, const Span &transformSpace) const -> Span
{
    return span;
}

auto NullTransform::validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool
{
    return true;
}

auto NullTransform::transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat
{
    return inputFormat;
}

auto NullTransform::transformBlockSize(std::size_t inputBufferSize) const noexcept -> std::size_t
{
    return inputBufferSize;
}

auto NullTransform::transformBlockSizeInverted(std::size_t outputBufferSize) const noexcept -> std::size_t
{
    return outputBufferSize;
}
