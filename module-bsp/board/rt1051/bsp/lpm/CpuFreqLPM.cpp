// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CpuFreqLPM.hpp"
#include <log/log.hpp>
#include "fsl_dcdc.h"

namespace bsp
{
    CpuFreqLPM::CpuFreqLPM()
    {}

    void CpuFreqLPM::SetCpuFrequency(CpuFreqLPM::CpuClock freq)
    {
        DCDC_AdjustTargetVoltage(DCDC, VDDRun_1275_mV, VDDStandby_925_mV);

        switch (freq) {
        case CpuClock::CpuClock_Osc_4_Mhz:
            /* Set PERIPH_CLK2_PODF. */
            CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 2);
            /* Set AHB_PODF. */
            CLOCK_SetDiv(kCLOCK_AhbDiv, 1); // CBCDR

            DCDC_AdjustTargetVoltage(DCDC, VDDRun_900_mV, VDDStandby_925_mV);
            break;
        case CpuClock::CpuClock_Osc_12_Mhz:
            /* Set PERIPH_CLK2_PODF. */
            CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
            /* Set AHB_PODF. */
            CLOCK_SetDiv(kCLOCK_AhbDiv, 1); // CBCDR

            DCDC_AdjustTargetVoltage(DCDC, VDDRun_900_mV, VDDStandby_925_mV);
            break;
        case CpuClock::CpuClock_Osc_24_Mhz:
            /* Set PERIPH_CLK2_PODF. */
            CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
            /* Set AHB_PODF. */
            CLOCK_SetDiv(kCLOCK_AhbDiv, 0); // CBCDR

            DCDC_AdjustTargetVoltage(DCDC, VDDRun_1050_mV, VDDStandby_925_mV);
            break;
        case CpuClock::CpuClock_Pll2_66_Mhz:
            /* Set AHB_PODF. */
            CLOCK_SetDiv(kCLOCK_AhbDiv, 7); // CBCDR
            DCDC_AdjustTargetVoltage(DCDC, VDDRun_1075_mV, VDDStandby_925_mV);
            break;
        case CpuClock::CpuClock_Pll2_132_Mhz:
            /* Set AHB_PODF. */
            CLOCK_SetDiv(kCLOCK_AhbDiv, 3); // CBCDR
            DCDC_AdjustTargetVoltage(DCDC, VDDRun_1100_mV, VDDStandby_925_mV);
            break;
        case CpuClock::CpuClock_Pll2_264_Mhz:
            /* Set AHB_PODF. */
            CLOCK_SetDiv(kCLOCK_AhbDiv, 1); // CBCDR
            DCDC_AdjustTargetVoltage(DCDC, VDDRun_1125_mV, VDDStandby_925_mV);
            break;
        case CpuClock::CpuClock_Pll2_528_Mhz:
            /* Set AHB_PODF. */
            CLOCK_SetDiv(kCLOCK_AhbDiv, 0); // CBCDR
            DCDC_AdjustTargetVoltage(DCDC, VDDRun_1150_mV, VDDStandby_925_mV);
            break;
        }

        if (freq == CpuClock::CpuClock_Osc_4_Mhz || freq == CpuClock::CpuClock_Osc_12_Mhz ||
            freq == CpuClock::CpuClock_Osc_24_Mhz) {
            /* Set periph clock source. */
            CLOCK_SetMux(kCLOCK_PeriphMux, 1);
        }
        else {
            /* Set periph clock source. */
            CLOCK_SetMux(kCLOCK_PeriphMux, 0);
        }

        /* Set SystemCoreClock variable. */
        SystemCoreClockUpdate();
    }

    void CpuFreqLPM::SetHighestCoreVoltage()
    {
        DCDC_AdjustTargetVoltage(DCDC, VDDRun_1275_mV, VDDStandby_925_mV);
    }

} // namespace bsp
