// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
