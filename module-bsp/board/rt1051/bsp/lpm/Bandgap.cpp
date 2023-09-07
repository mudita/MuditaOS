// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Bandgap.hpp"
#include <cstdint>
#include <fsl_common.h>
#include <critical.hpp>

namespace
{
    constexpr std::uint32_t REFTOP_LOWPOWER_MASK = (1U << 2);
}

namespace bsp::bandgap
{
    void SwitchToRegularMode()
    {
        cpp_freertos::CriticalSection::Enter();

        /* Enable regular bandgap and wait for it to stabilize */
        CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_REFTOP_PWD_MASK;
        while ((CCM_ANALOG->MISC0 & CCM_ANALOG_MISC0_REFTOP_VBGUP_MASK) == 0) {}

        /* Disable low power bandgap */
        XTALOSC24M->LOWPWR_CTRL_CLR = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL_MASK;
        PMU->MISC0_CLR              = REFTOP_LOWPOWER_MASK;

        cpp_freertos::CriticalSection::Exit();
    }

    void SwitchToLowPowerMode()
    {
        cpp_freertos::CriticalSection::Enter();

        /* Enable low power bandgap */
        PMU->MISC0_SET = REFTOP_LOWPOWER_MASK;
        XTALOSC24M->LOWPWR_CTRL_SET = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL_MASK;

        /* Disable regular bandgap */
        PMU->MISC0_SET              = CCM_ANALOG_MISC0_REFTOP_PWD_MASK;

        cpp_freertos::CriticalSection::Exit();
    }
} // namespace bsp::bandgap
