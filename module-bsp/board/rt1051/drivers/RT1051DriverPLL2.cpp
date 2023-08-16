// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPLL2.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"
#include "board/rt1051/bsp/lpm/Bandgap.hpp"
#include "board/rt1051/bsp/lpm/CpuFreqLPM.hpp"
#include <fsl_dcdc.h>

namespace drivers
{
    RT1051DriverPLL2::RT1051DriverPLL2() noexcept
    {
        if (!IsPLL2Enabled()) {
            // Set VDD_SOC_IN to 1.15V required to safely start PLL2
            DCDC_AdjustTargetVoltage(DCDC, bsp::VDDRun_1150_mV, bsp::VDDStandby_925_mV);

            // Switch to regular bandgap
            bsp::bandgap::SwitchToRegularMode();

            // Enable PLL2
            clkPLL2setup(CLK_ENABLE);
        }
    }

    RT1051DriverPLL2::~RT1051DriverPLL2()
    {
        if ((CLOCK_GetMux(kCLOCK_SemcMux) == SemcMuxPeripheralClock) && !bsp::IsClockEnabled(kCLOCK_Lpspi1) &&
            !bsp::IsClockEnabled(kCLOCK_Lpspi2) && !bsp::IsClockEnabled(kCLOCK_Lpspi3) &&
            !bsp::IsClockEnabled(kCLOCK_Lpspi4) && !bsp::IsClockEnabled(kCLOCK_Usdhc1) &&
            !bsp::IsClockEnabled(kCLOCK_Usdhc2)) {

            // Disable PLL2
            clkPLL2setup(CLK_DISABLE);

            // Switch to low power bandgap
            bsp::bandgap::SwitchToLowPowerMode();

            // After turning off PLL2 and with CPU @4MHZ VDD_SOC_IN can be set to 950mV
            DCDC_AdjustTargetVoltage(DCDC, bsp::VDDRun_950_mV, bsp::VDDStandby_925_mV);
        }
    }

    bool RT1051DriverPLL2::IsPLL2Enabled() const noexcept
    {
        return not static_cast<bool>(CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_POWERDOWN_MASK);
    }

} // namespace drivers
