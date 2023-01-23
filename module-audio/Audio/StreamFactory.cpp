// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StreamFactory.hpp"
#include "Endpoint.hpp"

#include "transcode/TransformFactory.hpp"

#include <Math.hpp>
#include <log/log.hpp>

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <memory>
#include <optional>

#include <cassert>
#include <cmath>

using audio::AudioFormat;
using audio::Sink;
using audio::Source;
using audio::Stream;
using audio::StreamFactory;
using audio::transcode::InputTranscodeProxy;
using audio::transcode::Transform;
using audio::transcode::TransformFactory;
using namespace std::chrono_literals;

StreamFactory::StreamFactory(std::chrono::milliseconds operationPeriodRequirement)
    : periodRequirement(operationPeriodRequirement)
{}

auto StreamFactory::makeStream(Source &source, Sink &sink) -> std::unique_ptr<AbstractStream>
{
    auto sourceFormat = source.getSourceFormat();
    auto sinkFormat   = sink.getSinkFormat();

    if (sourceFormat == sinkFormat) {
        return makeStream(source.getTraits(), sink.getTraits(), sourceFormat);
    }
    else {
        auto transformFactory = TransformFactory();
        auto transform        = transformFactory.makeTransform(sourceFormat, sinkFormat);

        return makeInputTranscodingStream(source, sink, sinkFormat, std::move(transform));
    }
}

auto StreamFactory::makeStream(Traits sourceTraits, Traits sinkTraits, AudioFormat streamFormat)
    -> std::unique_ptr<Stream>
{
    const auto endpointsTraits  = {sourceTraits, sinkTraits};
    const auto timingConstraint = getTimingConstraints(std::initializer_list<std::optional<std::chrono::milliseconds>>{
        sinkTraits.timeConstraint, sourceTraits.timeConstraint, periodRequirement});
    auto blockSizeConstraint = getBlockSizeConstraint(endpointsTraits);
    auto &streamAllocator    = negotiateAllocator(endpointsTraits);

    if (streamFormat == audio::nullFormat) {
        throw std::invalid_argument("No source format provided");
    }

    if (!blockSizeConstraint.has_value()) {
        blockSizeConstraint = binary::ceilPowerOfTwo(streamFormat.microsecondsToBytes(timingConstraint));
    }

    const auto blockTransferDuration =
        std::chrono::duration<double, std::milli>(streamFormat.bytesToMicroseconds(blockSizeConstraint.value()));
    const auto streamBuffering =
        static_cast<unsigned int>(std::ceil(timingConstraint / blockTransferDuration)) * defaultBuffering;

    LOG_DEBUG("Creating audio stream: block size = %lu bytes; buffering = %u -> stream buffer size = %lu bytes",
              static_cast<unsigned long>(blockSizeConstraint.value()),
              streamBuffering,
              static_cast<unsigned long>(blockSizeConstraint.value() * streamBuffering));

    return std::make_unique<Stream>(streamFormat, streamAllocator, blockSizeConstraint.value(), streamBuffering);
}

auto StreamFactory::makeStream(Source &source, Sink &sink, AudioFormat streamFormat) -> std::unique_ptr<Stream>
{
    return makeStream(source.getTraits(), sink.getTraits(), streamFormat);
}

auto StreamFactory::makeInputTranscodingStream(Source &source,
                                               Sink &sink,
                                               AudioFormat streamFormat,
                                               std::shared_ptr<Transform> transform)
    -> std::unique_ptr<InputTranscodeProxy>
{
    auto sourceTraits = source.getTraits();

    if (sourceTraits.blockSizeConstraint.has_value()) {
        sourceTraits.blockSizeConstraint = transform->transformBlockSize(sourceTraits.blockSizeConstraint.value());
    }

    auto stream            = makeStream(sourceTraits, sink.getTraits(), streamFormat);
    auto transcodingStream = std::make_unique<InputTranscodeProxy>(std::move(stream), transform);

    return transcodingStream;
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
