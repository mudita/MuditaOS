// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPM.hpp"
#include "WfiController.hpp"
#include <fsl_common.h>

namespace bsp
{
    void RT1051LPM::EnableDcdcPowerSaveMode()
    {}

    void RT1051LPM::DisableDcdcPowerSaveMode()
    {}

    void RT1051LPM::AllowEnteringWfiMode()
    {
        allowEnteringWfiMode();
    }

    void RT1051LPM::BlockEnteringWfiMode()
    {
        blockEnteringWfiMode();
    }

    std::uint32_t RT1051LPM::EnterWfiModeIfAllowed()
    {
        return enterWfiModeIfAllowed();
    }

    std::uint32_t RT1051LPM::GetLastTimeSpentInWfi()
    {
        return getLastTimeSpentInWfi();
    }

    void RT1051LPM::DisableSysTick()
    {
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        NVIC_ClearPendingIRQ(SysTick_IRQn);
    }

    void RT1051LPM::EnableSysTick()
    {
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    }

    std::uint32_t RT1051LPM::DisableInterrupts()
    {
        const auto primask = DisableGlobalIRQ();
        __DSB();
        __ISB();
        return primask;
    }

    void RT1051LPM::EnableInterrupts(std::uint32_t primask)
    {
        EnableGlobalIRQ(primask);
    }
} // namespace bsp
