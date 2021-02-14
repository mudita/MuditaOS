// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StreamFactory.hpp"
#include "Endpoint.hpp"

#include <math/Math.hpp>

#include <algorithm>
#include <memory>
#include <vector>

#include <cassert>

using namespace audio;

StreamFactory::StreamFactory(Endpoint::Capabilities factoryCaps, unsigned int bufferingSize)
    : caps(std::move(factoryCaps)), bufferingSize(bufferingSize)
{}

auto StreamFactory::makeStream(const Source &source, const Sink &sink) -> std::unique_ptr<Stream>
{
    auto negotiatedCaps = negotiateCaps({source, sink});

    return std::make_unique<Stream>(getAllocator(negotiatedCaps.usesDMA), negotiatedCaps.maxBlockSize, bufferingSize);
}

auto StreamFactory::negotiateCaps(std::vector<std::reference_wrapper<const Endpoint>> v) -> Endpoint::Capabilities
{
    auto negotiatedCaps = caps;

    for (const auto &endpointRef : v) {
        auto &endpointCaps = endpointRef.get().getCapabilities();

        negotiatedCaps.maxBlockSize = std::min(negotiatedCaps.maxBlockSize, endpointCaps.maxBlockSize);
        negotiatedCaps.minBlockSize = std::max(negotiatedCaps.minBlockSize, endpointCaps.minBlockSize);
        negotiatedCaps.usesDMA      = negotiatedCaps.usesDMA || endpointCaps.usesDMA;
    }

    negotiatedCaps.minBlockSize = binary::ceilPowerOfTwo(negotiatedCaps.minBlockSize);
    negotiatedCaps.maxBlockSize = binary::floorPowerOfTwo(negotiatedCaps.maxBlockSize);

    assert(negotiatedCaps.minBlockSize <= negotiatedCaps.maxBlockSize);

    return negotiatedCaps;
}

auto StreamFactory::getAllocator(bool usesDMA) -> Stream::Allocator &
{
    if (usesDMA) {
        return nonCacheableAlloc;
    }
    else {
        return stdAlloc;
    }
}
