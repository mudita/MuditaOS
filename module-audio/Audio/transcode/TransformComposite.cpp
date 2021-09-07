// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TransformComposite.hpp"
#include "Transform.hpp"

#include <Audio/AudioFormat.hpp>

#include <algorithm>
#include <memory>
#include <initializer_list>

using audio::transcode::Transform;
using audio::transcode::TransformComposite;

TransformComposite::TransformComposite(std::vector<std::shared_ptr<Transform>> transforms) : children(transforms)
{}

auto TransformComposite::transform(const Span &input, const Span &conversionSpace) const -> Span
{
    auto output = input;

    for (auto t : children) {
        output = t->transform(output, conversionSpace);
    }
    return output;
}

auto TransformComposite::validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool
{
    auto checkFormat = inputFormat;

    for (auto t : children) {
        if (!t->validateInputFormat(checkFormat)) {
            return false;
        }
        checkFormat = t->transformFormat(checkFormat);
    }

    return true;
}

auto TransformComposite::transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat
{
    auto outputFormat = inputFormat;

    for (auto t : children) {
        outputFormat = t->transformFormat(outputFormat);
    }

    return outputFormat;
}

auto TransformComposite::transformBlockSize(std::size_t blockSize) const noexcept -> std::size_t
{
    std::size_t transformedBlockSize = blockSize;

    for (auto t : children) {
        transformedBlockSize = t->transformBlockSize(transformedBlockSize);
    }

    return transformedBlockSize;
}

auto TransformComposite::transformBlockSizeInverted(std::size_t blockSize) const noexcept -> std::size_t
{
    std::size_t transformedBlockSize = blockSize;

    for (auto t : children) {
        transformedBlockSize = t->transformBlockSizeInverted(transformedBlockSize);
    }

    return transformedBlockSize;
}
