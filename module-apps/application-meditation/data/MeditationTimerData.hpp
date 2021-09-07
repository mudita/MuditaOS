// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>

#include <chrono>

namespace gui
{
    class MeditationTimerData : public gui::SwitchData
    {
      public:
        MeditationTimerData(std::chrono::seconds preparation,
                            std::chrono::seconds meditation,
                            std::chrono::seconds intervals,
                            bool showCounter)
            : preparationTime(preparation), meditationTime(meditation), meditationIntervals(intervals),
              showCounter(showCounter)
        {}

        [[nodiscard]] std::chrono::seconds getPreparationTime() const noexcept
        {
            return preparationTime;
        }
        [[nodiscard]] std::chrono::seconds getMeditationTime() const noexcept
        {
            return meditationTime;
        }
        [[nodiscard]] std::chrono::seconds getMeditationIntervals() const noexcept
        {
            return meditationIntervals;
        }
        [[nodiscard]] bool isCounterEnabled() const noexcept
        {
            return showCounter;
        }

      private:
        std::chrono::seconds preparationTime{};
        std::chrono::seconds meditationTime{};
        std::chrono::seconds meditationIntervals{};
        bool showCounter = true;
    };
}; // namespace gui
