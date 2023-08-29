// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPLL2.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"
#include "board/rt1051/bsp/lpm/CpuFreqLPM.hpp"
#include "board/rt1051/bsp/lpm/Bandgap.hpp"
#include "board/rt1051/bsp/lpm/LDO.hpp"
#include <fsl_dcdc.h>

namespace drivers
{
    RT1051DriverPLL2::RT1051DriverPLL2() noexcept
    {
        if (!IsPLL2Enabled()) {
            // Switch DCDC to CCM mode to improve stability
            //            DCDC_BootIntoCCM(DCDC);

            // Set VDD_SOC_IN to 1.15V required to safely start PLL2
            //            DCDC_AdjustTargetVoltage(DCDC, bsp::VDDRun_1150_mV, bsp::VDDStandby_925_mV);

            // Switch to regular bandgap
            //            bsp::bandgap::SwitchToRegularMode();

            // Switch to regular LDO
            //            bsp::ldo::SwitchToRegularMode();

            // Enable PLL2
            clkPLL2setup(CLK_ENABLE);
        }
    }

    RT1051DriverPLL2::~RT1051DriverPLL2() noexcept
    {
        //        if ((CLOCK_GetMux(kCLOCK_SemcMux) == SemcMuxPeripheralClock) && !bsp::IsClockEnabled(kCLOCK_Lpspi1) &&
        //            !bsp::IsClockEnabled(kCLOCK_Lpspi2) && !bsp::IsClockEnabled(kCLOCK_Lpspi3) &&
        //            !bsp::IsClockEnabled(kCLOCK_Lpspi4) && !bsp::IsClockEnabled(kCLOCK_Usdhc1) &&
        //            !bsp::IsClockEnabled(kCLOCK_Usdhc2)) {
        //
        //            // Disable PLL2
        //            clkPLL2setup(CLK_DISABLE);
        //
        //            // After turning off PLL2 and with CPU @4MHZ VDD_SOC_IN can be set to 950mV
        //            DCDC_AdjustTargetVoltage(DCDC, bsp::VDDRun_950_mV, bsp::VDDStandby_925_mV);
        //
        //            // Switch DCDC to DCM mode to reduce current consumption
        //            DCDC_BootIntoDCM(DCDC);
        //
        //            // Switch to low power LDO
        //            bsp::ldo::SwitchToLowPowerMode();
        //
        //            // Switch to low power bandgap
        //            bsp::bandgap::SwitchToLowPowerMode();
        //        }
    }

    bool RT1051DriverPLL2::IsPLL2Enabled() const noexcept
    {
        return not static_cast<bool>(CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_POWERDOWN_MASK);
    }
} // namespace drivers
