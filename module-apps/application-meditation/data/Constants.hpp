// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <chrono>
#include <array>

namespace Constants
{
    namespace Params
    {
        using minutes = std::chrono::minutes;
        constexpr auto defaultChimeInterval{minutes{2}};
        constexpr std::array<minutes, 6> chimeIntervals{
            minutes{2}, minutes{5}, minutes{10}, minutes{15}, minutes{30}, minutes{0}};

        constexpr int defaultMeditationDuration = 15;
        constexpr int minimalMeditationDuration = 1;
        constexpr int maximalMeditationDuration = 99;

        using namespace std::chrono_literals;
        constexpr auto defaultPreparationTime{5s};
        constexpr auto defaultCounterVisible{true};
        constexpr std::array<std::chrono::seconds, 7> preparationTimes{5s, 10s, 30s, 1min, 2min, 5min, 10min};
    } // namespace Params
} // namespace Constants
