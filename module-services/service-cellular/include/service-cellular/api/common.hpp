// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>

namespace cellular
{
    namespace service
    {
        constexpr const char *name = "ServiceCellular";
    }

    namespace api
    {
        enum class CallMode
        {
            Regular,
            Emergency
        };

        enum class SimLockState
        {
            Locked,
            Unlocked
        };

        enum class SimSlot
        {
            SIM1 = 0,
            SIM2 = 1
        };

        using SimCode = std::vector<unsigned int>;
    } // namespace api
} // namespace cellular
