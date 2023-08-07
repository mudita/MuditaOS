// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPLL2.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"
#include "board/rt1051/bsp/lpm/CpuFreqLPM.hpp"
#include "board/rt1051/bsp/lpm/Bandgap.hpp"
#include <fsl_dcdc.h>
#include <critical.hpp>

namespace
{
    constexpr auto ldoStabilizationDelayUs{40};

    inline void RegularLDOMode()
    {
        cpp_freertos::CriticalSection::Enter();

        // Disable weak 2P5 and enable regular 2P5 LDO
        PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
        PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_LINREG_MASK;

        // Wait for regular 2P5 to become stable (documentation Low Power AN12085)
        while ((PMU->REG_2P5 & PMU_REG_2P5_OK_VDD2P5_MASK) == 0) {}

        // Disable weak 1P1 and enable regular 1P1 LDO
        PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
        PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_LINREG_MASK;

        // Wait for regular 1P1 to become stable (documentation Low Power AN12085)
        while ((PMU->REG_1P1 & PMU_REG_1P1_OK_VDD1P1_MASK) == 0) {}

        cpp_freertos::CriticalSection::Exit();
    }

    inline void LowPowerLDOMode()
    {
        cpp_freertos::CriticalSection::Enter();

        // Disable regular 2P5 and enable weak 2P5 LDO
        PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_LINREG_MASK;
        PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;

        // Disable regular 1P1 and enable weak 1P1 LDO
        PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_LINREG_MASK;
        PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;

        // Wait for weak LDOs to stabilize
        SDK_DelayAtLeastUs(ldoStabilizationDelayUs, CLOCK_GetCpuClkFreq());

        cpp_freertos::CriticalSection::Exit();
    }
} // namespace

namespace drivers
{
    RT1051DriverPLL2::RT1051DriverPLL2() noexcept
    {
        if (!IsPLL2Enabled()) {
            // Switch DCDC to CCM mode to improve stability
            DCDC_BootIntoCCM(DCDC);

            // Set VDD_SOC_IN to 1.15V required to safely start PLL2
            DCDC_AdjustTargetVoltage(DCDC, bsp::VDDRun_1150_mV, bsp::VDDStandby_925_mV);

            // Switch to regular bandgap
            bsp::SwitchToRegularBandgap();

            // Switch to regular LDO
            RegularLDOMode();

            // Enable PLL2
            clkPLL2setup(CLK_ENABLE);
        }
    }

    RT1051DriverPLL2::~RT1051DriverPLL2() noexcept
    {
        if ((CLOCK_GetMux(kCLOCK_SemcMux) == SemcMuxPeripheralClock) && !bsp::IsClockEnabled(kCLOCK_Lpspi1) &&
            !bsp::IsClockEnabled(kCLOCK_Lpspi2) && !bsp::IsClockEnabled(kCLOCK_Lpspi3) &&
            !bsp::IsClockEnabled(kCLOCK_Lpspi4) && !bsp::IsClockEnabled(kCLOCK_Usdhc1) &&
            !bsp::IsClockEnabled(kCLOCK_Usdhc2)) {

            // Disable PLL2
            clkPLL2setup(CLK_DISABLE);

            // After turning off PLL2 and with CPU @4MHZ VDD_SOC_IN can be set to 950mV
            DCDC_AdjustTargetVoltage(DCDC, bsp::VDDRun_950_mV, bsp::VDDStandby_925_mV);

            // Switch DCDC to DCM mode to reduce current consumption
            DCDC_BootIntoDCM(DCDC);

            // Switch to low power LDO
            LowPowerLDOMode();

            // Switch to low power bandgap
            bsp::SwitchToLowPowerBandgap();
        }
    }

    bool RT1051DriverPLL2::IsPLL2Enabled() const noexcept
    {
        return not static_cast<bool>(CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_POWERDOWN_MASK);
    }
} // namespace drivers
