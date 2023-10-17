// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LDO.hpp"
#include <fsl_common.h>
#include <critical.hpp>

namespace
{
    constexpr auto ldoStabilizationDelayUs{40};
}

namespace bsp::ldo
{
    void SwitchToRegularMode()
    {
        cpp_freertos::CriticalSection::Enter();

        /* Enable regular 2P5 and disable weak 2P5 LDO */
        PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_LINREG_MASK;
        PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;

        /* Wait for regular 2P5 to become stable (documentation Low Power AN12085) */
        while ((PMU->REG_2P5 & PMU_REG_2P5_OK_VDD2P5_MASK) == 0) {}

        /* Enable regular 1P1 and disable weak 1P1 LDO */
        PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_LINREG_MASK;
        PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;

        /* Wait for regular 1P1 to become stable (documentation Low Power AN12085) */
        while ((PMU->REG_1P1 & PMU_REG_1P1_OK_VDD1P1_MASK) == 0) {}

        cpp_freertos::CriticalSection::Exit();

#if BOARD_BellHybrid == 1
        NVIC_ClearPendingIRQ(ANATOP_EVENT0_IRQn);
        EnableIRQ(ANATOP_EVENT0_IRQn);
#endif
    }

    void SwitchToLowPowerMode()
    {
#if BOARD_BellHybrid == 1
        DisableIRQ(ANATOP_EVENT0_IRQn);
#endif

        cpp_freertos::CriticalSection::Enter();

        /* Enable weak 2P5 and disable regular 2P5 LDO */
        PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
        PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_LINREG_MASK;

        /* Enable weak 1P1 and disable regular 1P1 LDO */
        PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
        PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_LINREG_MASK;

        /* Wait for weak LDOs to stabilize */
        SDK_DelayAtLeastUs(ldoStabilizationDelayUs, CLOCK_GetCpuClkFreq());

        cpp_freertos::CriticalSection::Exit();
    }
} // namespace bsp::ldo
