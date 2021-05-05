// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StreamFactory.hpp"
#include "Endpoint.hpp"

#include <math/Math.hpp>

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <memory>
#include <optional>

#include <cassert>
#include <cmath>

using audio::Sink;
using audio::Source;
using audio::Stream;
using audio::StreamFactory;
using namespace std::chrono_literals;

StreamFactory::StreamFactory(std::chrono::milliseconds operationPeriodRequirement)
    : periodRequirement(operationPeriodRequirement)
{}

auto StreamFactory::makeStream(Source &source, Sink &sink, AudioFormat streamFormat) -> std::unique_ptr<Stream>
{
    auto streamBuffering     = defaultBuffering;
    auto endpointsTraits     = {source.getTraits(), sink.getTraits()};
    auto blockSizeConstraint = getBlockSizeConstraint(endpointsTraits);
    auto &streamAllocator    = negotiateAllocator(endpointsTraits);
    auto timingConstraint    = getTimingConstraints(std::initializer_list<std::optional<std::chrono::milliseconds>>{
        sink.getTraits().timeConstraint, source.getTraits().timeConstraint, periodRequirement});

    if (streamFormat == audio::nullFormat) {
        throw std::invalid_argument("No source format provided");
    }

    if (!blockSizeConstraint.has_value()) {
        blockSizeConstraint = binary::ceilPowerOfTwo(streamFormat.microsecondsToBytes(timingConstraint));
    }

    auto blockTransferDuration =
        std::chrono::duration<double, std::milli>(streamFormat.bytesToMicroseconds(blockSizeConstraint.value()));
    streamBuffering = static_cast<unsigned int>(std::ceil(timingConstraint / blockTransferDuration)) * defaultBuffering;

    LOG_DEBUG("Creating audio stream: block size = %lu; buffering = %u",
              static_cast<unsigned long>(blockSizeConstraint.value()),
              streamBuffering);

    return std::make_unique<Stream>(streamFormat, streamAllocator, blockSizeConstraint.value(), streamBuffering);
}

auto StreamFactory::getBlockSizeConstraint(std::initializer_list<audio::Endpoint::Traits> traitsList) const
    -> std::optional<std::size_t>
{
    std::optional<std::size_t> blockSize = std::nullopt;

    for (const auto &traits : traitsList) {
        if (!traits.blockSizeConstraint.has_value()) {
            continue;
        }

        auto blockSizeCandidate = traits.blockSizeConstraint.value();
        if (blockSizeCandidate == 0) {
            throw std::invalid_argument("Invalid stream block size");
        }

        if (!blockSize.has_value()) {
            blockSize = blockSizeCandidate;
        }
        else if (blockSize != blockSizeCandidate) {
            throw std::invalid_argument("Block size mismatch");
        }
    }

    return blockSize;
}

auto StreamFactory::getTimingConstraints(
    std::initializer_list<std::optional<std::chrono::milliseconds>> timingConstraints) const
    -> std::chrono::milliseconds
{
    auto constraint = std::max(timingConstraints, [](const auto &left, const auto &right) {
        return right.value_or(std::chrono::milliseconds(0)).count() >
               left.value_or(std::chrono::milliseconds(0)).count();
    });

    if (!constraint.has_value()) {
        throw std::invalid_argument("At least one timing constraint must be provided");
    }

    return constraint.value();
}

auto StreamFactory::negotiateAllocator(std::initializer_list<audio::Endpoint::Traits> traitsList) noexcept
    -> Stream::Allocator &
{
    auto mustUseDMA =
        std::any_of(std::begin(traitsList), std::end(traitsList), [](const auto &trait) { return trait.usesDMA; });
    if (mustUseDMA) {
        LOG_DEBUG("Using fast memory allocator for audio streaming");
        return nonCacheableAlloc;
    }
    else {
        LOG_DEBUG("Using standard allocator for audio streaming");
        return stdAlloc;
    }
}
