// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bsp
{
    enum class RetCode{
        Success,
        Failure

    };

    /// CPU frequency is dependent on the clock settings.
    /// Only a few thresholds are available in the current configuration
    enum class CpuFrequencyHz
    {
        Level_0 = 4000000,
        Level_1 = 12000000,
        Level_2 = 24000000,
        Level_3 = 66000000,
        Level_4 = 132000000,
        Level_5 = 264000000,
        Level_6 = 528000000
    };

    enum class Board{
        RT1051,
    	Linux,
    	none
    };
    [[nodiscard]] const char *c_str(const Board &board);

}

