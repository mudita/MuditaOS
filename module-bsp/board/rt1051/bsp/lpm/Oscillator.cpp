// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Oscillator.hpp"
#include "ClockState.hpp"
#include <critical.hpp>
#include <fsl_dcdc.h>
#include <fsl_common.h>
#include <cstdint>

namespace
{
    constexpr std::uint8_t oscillatorReadyCounterValue{127};
    constexpr std::uint32_t currentTuningValueInUseForConfig0{0x4};
    constexpr std::uint32_t initialNegativeHysteresisValue{0x2};
    constexpr std::uint32_t negativeHysteresisValue{0x3};
    constexpr std::uint32_t positiveHysteresisValue{0x3};
    constexpr std::uint32_t tuningValue{0xA7};
    constexpr std::uint32_t currentTuningValueInUseForConfig1{0x40};
    constexpr std::uint32_t targetCountUsedToTune{0x2DC};

    constexpr std::uint32_t xtalStabilizationTimeUs{200};
    constexpr std::uint32_t rcOscStabilizationTimeUs{4000};
} // namespace

namespace bsp
{
    void SwitchToExternalOscillator()
    {
        cpp_freertos::CriticalSection::Enter();
        if (!IsExternalOscillatorEnabled()) {
            /// Init internal oscillator
            CLOCK_InitExternalClk(false);
            /// Wait for XTAL to become stable
            SDK_DelayAtLeastUs(xtalStabilizationTimeUs, CLOCK_GetCpuClkFreq());
            /// Switch DCDC to use DCDC external OSC
            DCDC_SetClockSource(DCDC, kDCDC_ClockExternalOsc);
            /// Switch clock source to external OSC.
            CLOCK_SwitchOsc(kCLOCK_XtalOsc);
            /// Deinit internal oscillator
            CLOCK_DeinitRcOsc24M();
            /// Set Oscillator ready counter value.
            CCM->CCR = (CCM->CCR & (~CCM_CCR_OSCNT_MASK)) | CCM_CCR_OSCNT(oscillatorReadyCounterValue);
        }
        cpp_freertos::CriticalSection::Exit();
    }

    bool SwitchToInternalOscillator()
    {
        cpp_freertos::CriticalSection::Enter();
        if (IsExternalOscillatorEnabled()) {
            if (IsClockEnabled(kCLOCK_Lpuart1) || IsClockEnabled(kCLOCK_Lpuart2) || IsClockEnabled(kCLOCK_Lpuart3) ||
                IsClockEnabled(kCLOCK_Lpuart4) || IsClockEnabled(kCLOCK_Lpuart5) || IsClockEnabled(kCLOCK_Lpuart6) ||
                IsClockEnabled(kCLOCK_Lpuart7) || IsClockEnabled(kCLOCK_Lpuart8)) {
                cpp_freertos::CriticalSection::Exit();
                return false;
            }

            /// Enable RC OSC. It needs at least 4ms to be stable, so self tuning need to be enabled.
            CLOCK_InitRcOsc24M();
            /// Configure self-tuning for RC OSC
            XTALOSC24M->OSC_CONFIG0 = XTALOSC24M_OSC_CONFIG0_RC_OSC_PROG_CUR(currentTuningValueInUseForConfig0) |
                                      XTALOSC24M_OSC_CONFIG0_SET_HYST_MINUS(initialNegativeHysteresisValue) |
                                      XTALOSC24M_OSC_CONFIG0_RC_OSC_PROG(tuningValue) |
                                      XTALOSC24M_OSC_CONFIG0_START_MASK | XTALOSC24M_OSC_CONFIG0_ENABLE_MASK;

            XTALOSC24M->OSC_CONFIG1 = XTALOSC24M_OSC_CONFIG1_COUNT_RC_CUR(currentTuningValueInUseForConfig1) |
                                      XTALOSC24M_OSC_CONFIG1_COUNT_RC_TRG(targetCountUsedToTune);

            /// Wait for RC OSC to become stable
            SDK_DelayAtLeastUs(rcOscStabilizationTimeUs, CLOCK_GetCpuClkFreq());
            /// Add hysteresis
            std::uint32_t oscConfig0Value = XTALOSC24M->OSC_CONFIG0;
            oscConfig0Value &= ~(XTALOSC24M_OSC_CONFIG0_HYST_PLUS_MASK | XTALOSC24M_OSC_CONFIG0_HYST_MINUS_MASK);
            oscConfig0Value |= XTALOSC24M_OSC_CONFIG0_HYST_PLUS(positiveHysteresisValue) |
                               XTALOSC24M_OSC_CONFIG0_HYST_MINUS(negativeHysteresisValue);
            XTALOSC24M->OSC_CONFIG0 = oscConfig0Value;
            /// Switch DCDC to use DCDC internal OSC
            DCDC_SetClockSource(DCDC, kDCDC_ClockInternalOsc);
            /// Switch clock source to internal RC
            CLOCK_SwitchOsc(kCLOCK_RcOsc);
            /// Deinit external oscillator
            CLOCK_DeinitExternalClk();
        }
        cpp_freertos::CriticalSection::Exit();

        return true;
    }

    [[nodiscard]] bool IsExternalOscillatorEnabled()
    {
        return not static_cast<bool>(XTALOSC24M->LOWPWR_CTRL & XTALOSC24M_LOWPWR_CTRL_OSC_SEL_MASK);
    }
}; // namespace bsp
