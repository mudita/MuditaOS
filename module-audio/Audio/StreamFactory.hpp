// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Endpoint.hpp"
#include "Stream.hpp"
#include "transcode/Transform.hpp"
#include "transcode/InputTranscodeProxy.hpp"

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

        auto makeStream(Source &source, Sink &sink) -> std::unique_ptr<AbstractStream>;
        auto makeStream(Source &source, Sink &sink, AudioFormat streamFormat) -> std::unique_ptr<Stream>;
        auto makeInputTranscodingStream(Source &source,
                                        Sink &sink,
                                        AudioFormat streamFormat,
                                        std::shared_ptr<transcode::Transform> transform)
            -> std::unique_ptr<transcode::InputTranscodeProxy>;

      private:
        using Traits = audio::Endpoint::Traits;

        static constexpr auto defaultBuffering = 24U;

        auto makeStream(Traits sourceTraits, Traits sinkTraits, AudioFormat streamFormat) -> std::unique_ptr<Stream>;

        auto getBlockSizeConstraint(std::initializer_list<Traits> traitsList) const -> std::optional<std::size_t>;
        auto getTimingConstraints(std::initializer_list<std::optional<std::chrono::milliseconds>> timingConstraints)
            const -> std::chrono::milliseconds;
        auto negotiateAllocator(std::initializer_list<Traits> traitsList) noexcept -> Stream::Allocator &;

        std::optional<std::chrono::milliseconds> periodRequirement = std::nullopt;

        StandardStreamAllocator stdAlloc;
        NonCacheableStreamAllocator nonCacheableAlloc;
    };

} // namespace audio
