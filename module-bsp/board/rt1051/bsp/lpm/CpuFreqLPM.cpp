// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CpuFreqLPM.hpp"
#include <fsl_dcdc.h>

namespace bsp
{
    CpuFreqLPM::CpuFreqLPM()
    {}

    void CpuFreqLPM::SetCpuFrequency(CpuFreqLPM::CpuClock freq)
    {
        switch (freq) {
        case CpuClock::CpuClock_Osc_4_Mhz:
            CLOCK_SetMux(kCLOCK_PeriphMux, PeriphMuxSourceOsc_24MHz);
            CLOCK_SetDiv(kCLOCK_AhbDiv, AhbDiv6);
            break;
        case CpuClock::CpuClock_Osc_12_Mhz:
            CLOCK_SetMux(kCLOCK_PeriphMux, PeriphMuxSourceOsc_24MHz);
            CLOCK_SetDiv(kCLOCK_AhbDiv, AhbDiv2);
            break;
        case CpuClock::CpuClock_Osc_24_Mhz:
            CLOCK_SetMux(kCLOCK_PeriphMux, PeriphMuxSourceOsc_24MHz);
            CLOCK_SetDiv(kCLOCK_AhbDiv, AhbDiv1);
            break;
        case CpuClock::CpuClock_Pll2_66_Mhz:
            CLOCK_SetDiv(kCLOCK_AhbDiv, AhbDiv8);
            CLOCK_SetMux(kCLOCK_PeriphMux, PeriphMuxSourcePll2_528MHz);
            break;
        case CpuClock::CpuClock_Pll2_132_Mhz:
            CLOCK_SetDiv(kCLOCK_AhbDiv, AhbDiv4);
            CLOCK_SetMux(kCLOCK_PeriphMux, PeriphMuxSourcePll2_528MHz);
            break;
        case CpuClock::CpuClock_Pll2_264_Mhz:
            CLOCK_SetDiv(kCLOCK_AhbDiv, AhbDiv2);
            CLOCK_SetMux(kCLOCK_PeriphMux, PeriphMuxSourcePll2_528MHz);
            break;
        case CpuClock::CpuClock_Pll2_528_Mhz:
            CLOCK_SetDiv(kCLOCK_AhbDiv, AhbDiv1);
            CLOCK_SetMux(kCLOCK_PeriphMux, PeriphMuxSourcePll2_528MHz);
            break;
        }

        /* Set SystemCoreClock variable. */
        SystemCoreClockUpdate();
    }
} // namespace bsp
