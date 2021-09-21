// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/cellular/SIM.hpp>

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
            Enabled,
            Disabled
        };

        using SimSlot = hal::cellular::SimSlot;

        enum class ModemState
        {
            Startup,
            Ready,
            Fail,
            Fatal
        };

        using SimCode = std::vector<unsigned int>;
    } // namespace api
} // namespace cellular
