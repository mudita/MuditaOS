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
            Enabled,
            Disabled
        };

        enum class SimSlot
        {
            SIM1,
            SIM2
        };

        enum class ModemState
        {
            Startup,
            Ready,
            Fail,
            Fatal
        };

        enum class SimState
        {
            Invalid, // Value in Settings is invalid, and so is currentSimSlot()
            Init,    // SimSlot selected, waiting for initialization result
            Ready,   // Initialization complete
            Fail     // Initialization failed
        };

        enum class TrayState
        {
            Inserted,
            Ejected
        };

        using SimCode = std::vector<unsigned int>;
    } // namespace api
} // namespace cellular
