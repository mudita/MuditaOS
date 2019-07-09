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

extern "C" {
#include "lpm.h"
#include "clock_config.h"
}

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
}

PowerMgr::~PowerMgr() {
    /* Recover to Full Run after suspend reset */
    LPM_SystemFullRun();
    LOG_INFO("[ServicePowerMgr] Cleaning resources");
}

sys::Message_t PowerMgr::DataReceivedHandler(sys::DataMessage *msgl) {

    std::shared_ptr<sys::ResponseMessage> responseMsg;

    return responseMsg;
}

// Invoked when timer ticked
void PowerMgr::TickHandler(uint32_t id) {
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