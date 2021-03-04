// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Oscillator.hpp"
#include "ClockState.hpp"
#include <fsl_dcdc.h>

namespace bsp
{
    inline constexpr uint8_t OscillatorReadyCounterValue{127};

    void EnableExternalOscillator()
    {
        if (!IsExternalOscillatorEnabled()) {
            CLOCK_InitExternalClk(0);
            /// Switch DCDC to use DCDC external OSC
            DCDC_SetClockSource(DCDC, kDCDC_ClockExternalOsc);
            /// Switch clock source to external OSC.
            CLOCK_SwitchOsc(kCLOCK_XtalOsc);
            /// Wait CCM operation finishes
            while (CCM->CDHIPR != 0) {}
            /// Set Oscillator ready counter value.
            CCM->CCR = (CCM->CCR & (~CCM_CCR_OSCNT_MASK)) | CCM_CCR_OSCNT(bsp::OscillatorReadyCounterValue);
        }
    }

    void DisableExternalOscillator()
    {
        if (IsExternalOscillatorEnabled()) {
            if (IsClockEnabled(kCLOCK_Lpuart1) || IsClockEnabled(kCLOCK_Lpuart2) || IsClockEnabled(kCLOCK_Lpuart3) ||
                IsClockEnabled(kCLOCK_Lpuart4) || IsClockEnabled(kCLOCK_Lpuart5) || IsClockEnabled(kCLOCK_Lpuart6) ||
                IsClockEnabled(kCLOCK_Lpuart7) || IsClockEnabled(kCLOCK_Lpuart8)) {
                return;
            }

            /// Switch DCDC to use DCDC internal OSC
            DCDC_SetClockSource(DCDC, kDCDC_ClockInternalOsc);
            /// Switch clock source to internal RC
            CLOCK_SwitchOsc(kCLOCK_RcOsc);
            CLOCK_DeinitExternalClk();
            /// Wait CCM operation finishes
            while (CCM->CDHIPR != 0) {}
        }
    }

    [[nodiscard]] bool IsExternalOscillatorEnabled()
    {
        return !(XTALOSC24M->LOWPWR_CTRL & XTALOSC24M_LOWPWR_CTRL_OSC_SEL_MASK);
    }
}; // namespace bsp
