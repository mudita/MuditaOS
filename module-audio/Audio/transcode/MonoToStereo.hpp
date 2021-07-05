// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Transform.hpp"

namespace audio::transcode
{
    /**
     * @brief Mono to stereo PCM16 data transform
     */
    class MonoToStereo : public Transform
    {
      public:
        auto transform(const Span &span, const Span &transformSpace) const -> Span override;
        auto validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool override;
        auto transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat override;
        auto transformBlockSize(std::size_t blockSize) const noexcept -> std::size_t override;
        auto transformBlockSizeInverted(std::size_t blockSize) const noexcept -> std::size_t override;
    };

} // namespace audio::transcode
