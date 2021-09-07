// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Transform.hpp"

#include <Audio/AudioFormat.hpp>

#include <memory>
#include <vector>

namespace audio::transcode
{

    /**
     * @brief Allows to execute a set of transforms to be performed in a chain.
     */
    class TransformComposite : public Transform
    {
      public:
        /**
         * @brief Construct a new Transform Composite object
         *
         * @param children - transforms to be executed on each transform call.
         */
        TransformComposite(std::vector<std::shared_ptr<Transform>> children);

        auto transform(const Span &span, const Span &transformSpace) const -> Span override;
        auto validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool override;
        auto transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat override;
        auto transformBlockSize(std::size_t blockSize) const noexcept -> std::size_t override;
        auto transformBlockSizeInverted(std::size_t blockSize) const noexcept -> std::size_t override;

      private:
        std::vector<std::shared_ptr<Transform>> children;
    };

} // namespace audio::transcode
