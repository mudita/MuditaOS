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

        enum class SimCardLock
        {
            Locked,
            Unlocked
        };

        enum class Sim
        {
            SIM1 = 0,
            SIM2 = 1
        };

        enum class PassCodeType
        {
            PIN,
            PUK
        };
        using PassCode = std::vector<unsigned int>;
    } // namespace api
} // namespace cellular
