// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DCDC.hpp"
#include "CpuFreqLPM.hpp"
#include <fsl_dcdc.h>

namespace bsp::dcdc
{
    void SwitchToOverdriveConfig()
    {
        /* Switch to CCM mode for improved stability */
        DCDC_BootIntoCCM(DCDC);

        /* Connect internal DCDC load resistor */
        DCDC->REG1 |= DCDC_REG1_REG_RLOAD_SW_MASK;

        /* Adjust target voltage to 1.275V */
        DCDC_AdjustTargetVoltage(DCDC, VDDRun_1275_mV, VDDStandby_925_mV);

        /* Disable FET ODRIVE */
        PMU->REG_CORE_CLR = PMU_REG_CORE_FET_ODRIVE_MASK;
    }

    void SwitchToRegularConfig()
    {
        /* Adjust target voltage to 1.15V */
        DCDC_AdjustTargetVoltage(DCDC, VDDRun_1150_mV, VDDStandby_925_mV);
    }

    void SwitchToLowPowerConfig()
    {
        /* Adjust target voltage to nominal 0.95V */
        DCDC_AdjustTargetVoltage(DCDC, VDDRun_950_mV, VDDStandby_925_mV);

        /* Switch to DCM mode to reduce current consumption */
        DCDC_BootIntoDCM(DCDC);

        /* Disconnect internal DCDC load resistor */
        DCDC->REG1 &= ~DCDC_REG1_REG_RLOAD_SW_MASK;

        /* Power down output range comparator */
        DCDC->REG0 |= DCDC_REG0_PWD_CMP_OFFSET_MASK;

        /* Enable FET ODRIVE */
        PMU->REG_CORE_SET = PMU_REG_CORE_FET_ODRIVE_MASK;
    }
} // namespace bsp::dcdc
