#pragma once

#include "gui/SwitchData.hpp"

namespace gui
{
    class MeditationTimerData : public gui::SwitchData
    {
      public:
        MeditationTimerData(int preparationInSec, int meditationInSec, int intervals, bool showCounter)
            : preparationTimeInSeconds(preparationInSec), meditationTimeInSeconds(meditationInSec),
              meditationIntervals(intervals), showCounter(showCounter)
        {}

        int getPreparationTimeInSeconds() const noexcept
        {
            return preparationTimeInSeconds;
        };
        int getMeditationTimeInSeconds() const noexcept
        {
            return meditationTimeInSeconds;
        };
        int getMeditationIntervals() const noexcept
        {
            return meditationIntervals;
        };
        bool getShowCounter() const noexcept
        {
            return showCounter;
        }

      private:
        int preparationTimeInSeconds = 0;
        int meditationTimeInSeconds  = 0;
        int meditationIntervals      = 0;
        bool showCounter             = true;
    };
}; // namespace gui
