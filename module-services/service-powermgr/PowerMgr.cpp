/*
 * @file PowerMgr.cpp
 * @author Lukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 * @date 08.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "PowerMgr.hpp"
#include "log/log.hpp"

extern "C" {
#include "lpm.h"
#include "clock_config.h"
}

PowerMgr::PowerMgr() {
    LOG_INFO("[ServicePowerMgr] Initializing");

    /* Boot ROM did initialize the XTAL, here we only sets external XTAL OSC freq */
    CLOCK_SetXtalFreq(BOARD_XTAL0_CLK_HZ);
    CLOCK_SetRtcXtalFreq(BOARD_XTAL32K_CLK_HZ);

    /* Restore when wakeup from suspend reset */
    LPM_SystemResumeDsm();

    /* Recover handshaking */
    IOMUXC_GPR->GPR4 = 0x00000000;
    IOMUXC_GPR->GPR7 = 0x00000000;
    IOMUXC_GPR->GPR8 = 0x00000000;
    IOMUXC_GPR->GPR12 = 0x00000000;

    CCM->CCR &= ~CCM_CCR_REG_BYPASS_COUNT_MASK;

    APP_PowerPostSwitchHook(LPM_PowerModeSuspend);

    /* Recover to Full Run after suspend reset */
    LPM_SystemFullRun();

    if (true != LPM_Init(LPM_PowerModeFullRun))
    {
        LOG_FATAL("LPM Init Failed!");
        return;
    }
    APP_PrintRunFrequency();
}

PowerMgr::~PowerMgr() {
    /* Recover to Full Run after suspend reset */
    LPM_SystemFullRun();
    LOG_INFO("[ServicePowerMgr] Cleaning resources");
}

void PowerMgr::APP_PrintRunFrequency()
{
    LOG_INFO("***********************************************************");
    LOG_INFO("CPU:             %d Hz", CLOCK_GetFreq(kCLOCK_CpuClk));
    LOG_INFO("AHB:             %d Hz", CLOCK_GetFreq(kCLOCK_AhbClk));
    LOG_INFO("SEMC:            %d Hz", CLOCK_GetFreq(kCLOCK_SemcClk));
    LOG_INFO("IPG:             %d Hz", CLOCK_GetFreq(kCLOCK_IpgClk));
    LOG_INFO("OSC:             %d Hz", CLOCK_GetFreq(kCLOCK_OscClk));
    LOG_INFO("RTC:             %d Hz", CLOCK_GetFreq(kCLOCK_RtcClk));
    LOG_INFO("ARMPLL:          %d Hz", CLOCK_GetFreq(kCLOCK_ArmPllClk));

    LOG_INFO("USB1PLL:         %d Hz", CLOCK_GetFreq(kCLOCK_Usb1PllClk));
    LOG_INFO("USB1PLLPFD0:     %d Hz", CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk));
    LOG_INFO("USB1PLLPFD1:     %d Hz", CLOCK_GetFreq(kCLOCK_Usb1PllPfd1Clk));
    LOG_INFO("USB1PLLPFD2:     %d Hz", CLOCK_GetFreq(kCLOCK_Usb1PllPfd2Clk));
    LOG_INFO("USB1PLLPFD3:     %d Hz", CLOCK_GetFreq(kCLOCK_Usb1PllPfd3Clk));
    LOG_INFO("USB2PLL:         %d Hz", CLOCK_GetFreq(kCLOCK_Usb2PllClk));
    LOG_INFO("SYSPLL:          %d Hz", CLOCK_GetFreq(kCLOCK_SysPllClk));
    LOG_INFO("SYSPLLPFD0:      %d Hz", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    LOG_INFO("SYSPLLPFD1:      %d Hz", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    LOG_INFO("SYSPLLPFD2:      %d Hz", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    LOG_INFO("SYSPLLPFD3:      %d Hz", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));
    LOG_INFO("ENETPLL0:        %d Hz", CLOCK_GetFreq(kCLOCK_EnetPll0Clk));
    LOG_INFO("ENETPLL1:        %d Hz", CLOCK_GetFreq(kCLOCK_EnetPll1Clk));
    LOG_INFO("AUDIOPLL:        %d Hz", CLOCK_GetFreq(kCLOCK_AudioPllClk));
    LOG_INFO("VIDEOPLL:        %d Hz", CLOCK_GetFreq(kCLOCK_VideoPllClk));

    LOG_INFO("***********************************************************");
}

void PowerMgr::SetPowerMode(PowerMode_t mode) {
	APP_SetLPMPowerMode(static_cast<lpm_power_mode_t>(mode));

	switch (mode)
    {
    case PowerModeOverRun:
        LPM_SystemOverRun();
        break;
    case PowerModeFullRun:
        LPM_SystemFullRun();
        break;
    case PowerModeLowSpeedRun:
        LPM_SystemLowSpeedRun();
        break;
    case PowerModeLowPowerRun:
        LPM_SystemLowPowerRun();
        break;
    default:
        break;
    }

}
