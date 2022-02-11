// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
namespace bsp
{
    enum class RetCode{
        Success,
        Failure

    };

    /// CPU frequency is dependent on the clock settings.
    /// Only a few thresholds are available in the current configuration
    enum class CpuFrequencyMHz
    {
        Level_0 = 4,
        Level_1 = 12,
        Level_2 = 24,
        Level_3 = 66,
        Level_4 = 132,
        Level_5 = 264,
        Level_6 = 528
    };

    uint8_t CpuMHZToLevel(enum CpuFrequencyMHz val);

    constexpr auto MHz_frequency_multiplier = 1000000U;

    enum class Board{
        RT1051,
    	Linux,
    	none
    };
    [[nodiscard]] const char *c_str(const Board &board);

}

