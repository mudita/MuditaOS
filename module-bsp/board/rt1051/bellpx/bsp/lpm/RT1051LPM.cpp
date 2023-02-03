// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPM.hpp"
#include <board.h>
#include <board/BoardDefinitions.hpp>

namespace bsp
{
    void RT1051LPM::EnableDcdcPowerSaveMode()
    {}

    void RT1051LPM::DisableDcdcPowerSaveMode()
    {}

    void RT1051LPM::SwitchToRegularModeLDO()
    {
        RT1051LPMCommon::RegularLDOMode();
        NVIC_ClearPendingIRQ(ANATOP_EVENT0_IRQn);
        EnableIRQ(ANATOP_EVENT0_IRQn);
    }

    void RT1051LPM::SwitchToLowPowerModeLDO()
    {
        DisableIRQ(ANATOP_EVENT0_IRQn);
        RT1051LPMCommon::LowPowerLDOMode();
    }
} // namespace bsp
