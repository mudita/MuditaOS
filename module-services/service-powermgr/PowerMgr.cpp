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
}

PowerMgr::PowerMgr() {
    LOG_INFO("[ServicePowerMgr] Initializing");

    /* Restore when wakeup from suspend reset */
    LPM_SystemResumeDsm();

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
