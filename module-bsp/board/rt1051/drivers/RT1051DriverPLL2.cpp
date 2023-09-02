// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPLL2.hpp"
#include "board/rt1051/bsp/lpm/ClockState.hpp"
#include "board/rt1051/bsp/lpm/CpuFreqLPM.hpp"
#include "board/rt1051/bsp/lpm/Bandgap.hpp"
#include "board/rt1051/bsp/lpm/LDO.hpp"
#include "board/rt1051/bsp/lpm/DCDC.hpp"

namespace drivers
{
    RT1051DriverPLL2::RT1051DriverPLL2() noexcept
    {
        if (!IsPLL2Enabled()) {

            //            LOG_ERROR("Enabling PLL2 with:");
            //            LOG_ERROR("\t> DCDC mode: %s",  (DCDC->REG0 & DCDC_REG0_PWD_ZCD_MASK) ? "CCM" : "DCM");
            //            LOG_ERROR("\t> core voltage: %lumV", 800 + 25 * (DCDC->REG3 & DCDC_REG3_TRG_MASK));
            //            LOG_ERROR("\t> bandgap mode: %s", (CCM_ANALOG->MISC0 & CCM_ANALOG_MISC0_REFTOP_PWD_MASK) ?
            //            "low power" : "regular"); LOG_ERROR("\t> LDO 2P5 mode: %s", (PMU->REG_2P5 &
            //            PMU_REG_2P5_ENABLE_LINREG_MASK) ? "regular" : "low power"); LOG_ERROR("\t> LDO 1P1 mode: %s",
            //            (PMU->REG_1P1 & PMU_REG_1P1_ENABLE_LINREG_MASK) ? "regular" : "low power");

            /* Check if switching conditions already met */
            const auto isCurrentVoltageOverdrive = ((DCDC->REG3 & DCDC_REG3_TRG_MASK) >= bsp::VDDRun_1275_mV);
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

            //            LOG_ERROR("Disabling PLL2 with:");
            //            LOG_ERROR("\t> DCDC mode: %s",  (DCDC->REG0 & DCDC_REG0_PWD_ZCD_MASK) ? "CCM" : "DCM");
            //            LOG_ERROR("\t> core voltage: %lumV", 800 + 25 * (DCDC->REG3 & DCDC_REG3_TRG_MASK));
            //            LOG_ERROR("\t> bandgap mode: %s", (CCM_ANALOG->MISC0 & CCM_ANALOG_MISC0_REFTOP_PWD_MASK) ?
            //            "low power" : "regular"); LOG_ERROR("\t> LDO 2P5 mode: %s", (PMU->REG_2P5 &
            //            PMU_REG_2P5_ENABLE_LINREG_MASK) ? "regular" : "low power"); LOG_ERROR("\t> LDO 1P1 mode: %s",
            //            (PMU->REG_1P1 & PMU_REG_1P1_ENABLE_LINREG_MASK) ? "regular" : "low power");

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
