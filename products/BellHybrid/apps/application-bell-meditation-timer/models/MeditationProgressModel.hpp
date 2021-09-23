// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MeditationBaseModel.hpp"

namespace app::meditation
{
    class MeditationProgressModel : public MeditationBaseModel
    {
      public:
        void onElapsed(std::chrono::seconds elapsed = std::chrono::seconds{1});
        auto getDuration() -> std::chrono::seconds;
        auto getInterval() -> std::chrono::seconds;
        auto getElapsed() -> std::chrono::seconds;
    };
} // namespace app::meditation
