// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Bandgap.hpp"
#include "fsl_common.h"

namespace
{
    constexpr auto REFTOP_LOWPOWER_FLAG = (1U << 2);
}

namespace bsp
{
    void SwitchToRegularBandgap()
    {
        // Enable regular bandgap
        CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_REFTOP_PWD_MASK;

        // Wait for regular bandgap to stabilize
        while ((CCM_ANALOG->MISC0 & CCM_ANALOG_MISC0_REFTOP_VBGUP_MASK) == 0) {}

        // Disable low power bandgap
        XTALOSC24M->LOWPWR_CTRL_CLR = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL_MASK;
        PMU->MISC0_CLR              = REFTOP_LOWPOWER_FLAG;
    }

    void SwitchToLowPowerBandgap()
    {
        // Enable low power bandgap
        PMU->MISC0_SET              = REFTOP_LOWPOWER_FLAG;
        XTALOSC24M->LOWPWR_CTRL_SET = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL_MASK;

        // Disable regular bandgap
        PMU->MISC0_SET = CCM_ANALOG_MISC0_REFTOP_PWD_MASK;
    }
} // namespace bsp
