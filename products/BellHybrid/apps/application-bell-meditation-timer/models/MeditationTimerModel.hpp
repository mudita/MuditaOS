// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MeditationBaseModel.hpp"

namespace app::meditation
{
    class MeditationTimerModel : public MeditationBaseModel
    {
      private:
        std::function<void()> onTimerChangedCallback{nullptr};

      public:
        auto setOnTimerChanged(std::function<void()> cb) -> void;
        [[nodiscard]] auto getTimer() -> std::chrono::seconds;
        void increaseTimer();
        void decreaseTimer();
        auto getTimerString() -> std::string;
    };
} // namespace app::meditation
