// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Oscillator.hpp"
#include "ClockState.hpp"
#include <fsl_dcdc.h>
#include <cstdint>

namespace bsp
{
    inline constexpr std::uint8_t OscillatorReadyCounterValue{127};
    inline constexpr std::uint32_t CurrentTuningValueInUseForConfig0{0x4};
    inline constexpr std::uint32_t NegativeHysteresisValue{0x2};
    inline constexpr std::uint32_t TuningValue{0xA7};
    inline constexpr std::uint32_t CurrentTuningValueInUseForConfig1{0x40};
    inline constexpr std::uint32_t TargetCountUsedToTune{0x2DC};

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

            // Enable RC OSC. It needs at least 4ms to be stable, so self tuning need to be enabled.
            XTALOSC24M->LOWPWR_CTRL |= XTALOSC24M_LOWPWR_CTRL_RC_OSC_EN_MASK;
            // Configure self-tuning for RC OSC
            XTALOSC24M->OSC_CONFIG0 = XTALOSC24M_OSC_CONFIG0_RC_OSC_PROG_CUR(bsp::CurrentTuningValueInUseForConfig0) |
                                      XTALOSC24M_OSC_CONFIG0_SET_HYST_MINUS(bsp::NegativeHysteresisValue) |
                                      XTALOSC24M_OSC_CONFIG0_RC_OSC_PROG(bsp::TuningValue) |
                                      XTALOSC24M_OSC_CONFIG0_START_MASK | XTALOSC24M_OSC_CONFIG0_ENABLE_MASK;
            XTALOSC24M->OSC_CONFIG1 = XTALOSC24M_OSC_CONFIG1_COUNT_RC_CUR(bsp::CurrentTuningValueInUseForConfig1) |
                                      XTALOSC24M_OSC_CONFIG1_COUNT_RC_TRG(bsp::TargetCountUsedToTune);

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
