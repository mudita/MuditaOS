// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"

#include <memory>
#include <vector>

namespace audio
{

    class StreamFactory
    {
      public:
        explicit StreamFactory(Endpoint::Capabilities factoryCaps,
                               unsigned int bufferingSize = Stream::defaultBufferingSize);
        auto makeStream(const Source &source, const Sink &sink) -> std::unique_ptr<Stream>;

      private:
        auto negotiateCaps(std::vector<std::reference_wrapper<const Endpoint>> v) -> Endpoint::Capabilities;
        auto getAllocator(bool usesDMA) -> Stream::Allocator &;

        Endpoint::Capabilities caps;
        StandardStreamAllocator stdAlloc;
        NonCacheableStreamAllocator nonCacheableAlloc;
        unsigned int bufferingSize;
    };

} // namespace audio
