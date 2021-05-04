// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"
#include "Stream.hpp"

#include <initializer_list>
#include <optional>
#include <memory>

namespace audio
{

    class StreamFactory
    {
      public:
        StreamFactory() = default;
        explicit StreamFactory(std::chrono::milliseconds operationPeriodRequirement);
        auto makeStream(Source &source, Sink &sink, AudioFormat streamFormat) -> std::unique_ptr<Stream>;

      private:
        static constexpr auto defaultBuffering = 2U;

        auto getBlockSizeConstraint(std::initializer_list<audio::Endpoint::Traits> traitsList) const
            -> std::optional<std::size_t>;
        auto getTimingConstraints(std::initializer_list<std::optional<std::chrono::milliseconds>> timingConstraints)
            const -> std::chrono::milliseconds;
        auto negotiateAllocator(std::initializer_list<audio::Endpoint::Traits> traitsList) noexcept
            -> Stream::Allocator &;

        std::optional<std::chrono::milliseconds> periodRequirement = std::nullopt;

        StandardStreamAllocator stdAlloc;
        NonCacheableStreamAllocator nonCacheableAlloc;
    };

} // namespace audio
