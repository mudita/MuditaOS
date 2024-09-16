// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Audio/AudioFormat.hpp>
#include <Audio/transcode/Transform.hpp>

#include <memory>

namespace audio::transcode
{
    class TransformFactory
    {
      public:
        auto makeTransform(AudioFormat sourceFormat, AudioFormat sinkFormat) const -> std::unique_ptr<Transform>;

      private:
        auto getSamplerateTransform(AudioFormat sourceFormat, AudioFormat sinkFormat) const
            -> std::unique_ptr<Transform>;
        auto getChannelsTransform(AudioFormat sourceFormat, AudioFormat sinkFormat) const -> std::unique_ptr<Transform>;
    };

}; // namespace audio::transcode
