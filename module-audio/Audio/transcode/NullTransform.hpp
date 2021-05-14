#pragma once

#include "Transform.hpp"

namespace audio::transcode
{

    class NullTransform : public Transform
    {
      public:
        auto transform(const Span &span, const Span &transformSpace) const -> Span override;
        auto validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool override;
        auto transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat override;
        auto transformBlockSize(std::size_t inputBufferSize) const noexcept -> std::size_t override;
        auto transformBlockSizeInversed(std::size_t outputBufferSize) const noexcept -> std::size_t override;
    };

} // namespace audio::transcode
