// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MeditationBaseModel.hpp"

namespace app::meditation
{
    class IntervalChimeModel : public MeditationBaseModel
    {
      private:
        enum class IntervalType
        {
            IntervalNone = 0,
            Interval_1,
            Interval_2,
            Interval_5,
            Interval_10,
            Interval_15
        };

        std::function<void()> onIntervalChangedCallback{nullptr};
        IntervalType interval{IntervalType::IntervalNone};

        IntervalType secsToInterval(std::chrono::seconds secs);
        std::chrono::seconds intervalToSecs(IntervalType interval);

      public:
        auto setOnIntervalChanged(std::function<void()> cb) -> void;
        [[nodiscard]] auto getInterval() -> std::chrono::seconds;
        void nextInterval();
        void previousInterval();
        auto getIntervalString() -> std::string;
    };
} // namespace app::meditation
