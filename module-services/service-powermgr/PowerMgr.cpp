/*
 * @file PowerMgr.cpp
 * @author Lukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 * @date 08.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "PowerMgr.hpp"
#include "MessageType.hpp"
#include "Database/Database.hpp"
#include "log/log.hpp"
#include "ticks.hpp"
#include "service-appmgr/ApplicationManager.hpp"

extern "C" {
#include "lpm.h"
#include "clock_config.h"
}

using namespace sapm;

const char *PowerMgr::serviceName = "ServicePowerMgr";


PowerMgr::PowerMgr()
        : sys::Service(serviceName, 1024 * 24, sys::ServicePriority::Idle) {
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
    APP_PrintRunFrequency(0);

    timer_id = CreateTimer(10000, true);
    ReloadTimer(timer_id);
}

PowerMgr::~PowerMgr() {
    /* Recover to Full Run after suspend reset */
    LPM_SystemFullRun();
    LOG_INFO("[ServicePowerMgr] Cleaning resources");
}

sys::Message_t PowerMgr::DataReceivedHandler(sys::DataMessage *msgl) {

    std::shared_ptr<sys::ResponseMessage> responseMsg;

    responseMsg = std::make_shared<sys::ResponseMessage>( );

    return responseMsg;
}

// Invoked when timer ticked
void PowerMgr::TickHandler(uint32_t id) {
    switch(pmode) {
        case 0:
            SetPowerMode(LPM_PowerModeLowSpeedRun);
            pmode = 1;
            break;
        case 1:
            SetPowerMode(LPM_PowerModeLowPowerRun);
            pmode = 2;
            break;
        case 2:
            SetPowerMode(LPM_PowerModeFullRun);
            pmode = 0;
            break;
    }
    APP_PrintRunFrequency(0);
}

// Invoked during initialization
sys::ReturnCodes PowerMgr::InitHandler() {

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes PowerMgr::DeinitHandler() {

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes PowerMgr::WakeUpHandler() {
    return sys::ReturnCodes::Success;
}


sys::ReturnCodes PowerMgr::SleepHandler() {
    return sys::ReturnCodes::Success;
}

void PowerMgr::APP_PrintRunFrequency(int32_t run_freq_only)
{
    LOG_INFO("***********************************************************");
    LOG_INFO("CPU:             %d Hz", CLOCK_GetFreq(kCLOCK_CpuClk));
    LOG_INFO("AHB:             %d Hz", CLOCK_GetFreq(kCLOCK_AhbClk));
    LOG_INFO("SEMC:            %d Hz", CLOCK_GetFreq(kCLOCK_SemcClk));
    LOG_INFO("IPG:             %d Hz", CLOCK_GetFreq(kCLOCK_IpgClk));
    LOG_INFO("OSC:             %d Hz", CLOCK_GetFreq(kCLOCK_OscClk));
    LOG_INFO("RTC:             %d Hz", CLOCK_GetFreq(kCLOCK_RtcClk));
    LOG_INFO("ARMPLL:          %d Hz", CLOCK_GetFreq(kCLOCK_ArmPllClk));
    if (!run_freq_only)
    {
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
    }
    LOG_INFO("***********************************************************");
}

void PowerMgr::SetPowerMode(lpm_power_mode_t mode) {
	APP_SetLPMPowerMode(mode);

	switch (mode)
    {
    case LPM_PowerModeOverRun:
        LPM_SystemOverRun();
        break;
    case LPM_PowerModeFullRun:
        LPM_SystemFullRun();
        break;
    case LPM_PowerModeLowSpeedRun:
        LPM_SystemLowSpeedRun();
        break;
    case LPM_PowerModeLowPowerRun:
        LPM_SystemLowPowerRun();
        break;
    default:
        break;
    }

}

bool PowerMgr::messageSwitchApplication( sys::Service* sender, const std::string& applicationName, const std::string& windowName, std::unique_ptr<gui::SwitchData> data ) {

	auto msg = std::make_shared<sapm::APMSwitch>( sender->GetName(), applicationName, windowName, std::move(data) );
	sys::Bus::SendUnicast(msg, "ServicePowerMgrr", sender);
	return true;
}

bool PowerMgr::messageConfirmSwitch( sys::Service* sender) {

	auto msg = std::make_shared<sapm::APMConfirmSwitch>(sender->GetName() );

	auto ret =  sys::Bus::SendUnicast(msg, "ServicePowerMgr", sender,2000  );
	return (ret.first == sys::ReturnCodes::Success )?true:false;
}
bool PowerMgr::messageConfirmClose( sys::Service* sender) {

	auto msg = std::make_shared<sapm::APMConfirmClose>(sender->GetName() );
	auto ret = sys::Bus::SendUnicast(msg, "ServicePowerMgr", sender, 500);
	return (ret.first == sys::ReturnCodes::Success )?true:false;
}
bool PowerMgr::messageSwitchPreviousApplication( sys::Service* sender ) {

	auto msg = std::make_shared<sapm::APMSwitchPrevApp>(sender->GetName() );
	auto ret = sys::Bus::SendUnicast(msg, "ServicePowerMgr", sender, 500);
	return (ret.first == sys::ReturnCodes::Success )?true:false;
}

bool PowerMgr::messageRegisterApplication( sys::Service* sender, const bool& status ) {
	auto msg = std::make_shared<sapm::APMRegister>(sender->GetName(), status );
	sys::Bus::SendUnicast(msg, "ServicePowerMgr", sender  );
	return true;
}
