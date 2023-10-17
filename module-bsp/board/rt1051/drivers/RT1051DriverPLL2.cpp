// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPLL2.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"
#include "board/rt1051/bsp/lpm/CpuFreqLPM.hpp"
#include "board/rt1051/bsp/lpm/Bandgap.hpp"
#include "board/rt1051/bsp/lpm/LDO.hpp"
#include "board/rt1051/bsp/lpm/DCDC.hpp"

namespace
{
    inline std::uint32_t getCurrentVDDRunVoltage()
    {
        return (DCDC->REG3 & DCDC_REG3_TRG_MASK);
    }
} // namespace

namespace drivers
{
    RT1051DriverPLL2::RT1051DriverPLL2() noexcept
    {
        if (!IsPLL2Enabled()) {
            /* Check if switching conditions already met */
            const auto isCurrentVoltageOverdrive = (getCurrentVDDRunVoltage() >= bsp::VDDRun_1275_mV);
            if (!isCurrentVoltageOverdrive) {
                bsp::dcdc::SwitchToOverdriveConfig();
            }

            /* Switch to regular bandgap */
            bsp::bandgap::SwitchToRegularMode();

            /* Switch to regular LDO */
            bsp::ldo::SwitchToRegularMode();

            /* Switch back to regular config */
            if (!isCurrentVoltageOverdrive) {
                bsp::dcdc::SwitchToRegularConfig();
            }

            /* Enable PLL2 */
            clkPLL2setup(CLK_ENABLE);
        }
    }

    RT1051DriverPLL2::~RT1051DriverPLL2() noexcept
    {
        if ((CLOCK_GetMux(kCLOCK_SemcMux) == SemcMuxPeripheralClock) && !bsp::IsClockEnabled(kCLOCK_Lpspi1) &&
            !bsp::IsClockEnabled(kCLOCK_Lpspi2) && !bsp::IsClockEnabled(kCLOCK_Lpspi3) &&
            !bsp::IsClockEnabled(kCLOCK_Lpspi4) && !bsp::IsClockEnabled(kCLOCK_Usdhc1) &&
            !bsp::IsClockEnabled(kCLOCK_Usdhc2)) {
            /* Disable PLL2 */
            clkPLL2setup(CLK_DISABLE);

            /* Switch DCDC to low power config */
            bsp::dcdc::SwitchToLowPowerConfig();

            /* Switch to low power LDO */
            bsp::ldo::SwitchToLowPowerMode();

            /* Switch to low power bandgap */
            bsp::bandgap::SwitchToLowPowerMode();
        }
    }

    bool RT1051DriverPLL2::IsPLL2Enabled() const noexcept
    {
        return not static_cast<bool>(CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_POWERDOWN_MASK);
    }
} // namespace drivers
