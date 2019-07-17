
/*
 * @file PowerMgr.hpp
 * @author Lukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 * @date 08.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_POWERMGR_HPP
#define PUREPHONE_POWERMGR_HPP

extern "C" {
#include "lpm.h"
}

class PowerMgr {
private:
    uint32_t pmode = 0;

protected:

public:
    PowerMgr();
    ~PowerMgr();

    enum PowerMode_t {
    	PowerModeOverRun = LPM_PowerModeOverRun,
		PowerModeFullRun = LPM_PowerModeFullRun,
		PowerModeLowSpeedRun = LPM_PowerModeLowSpeedRun,
		PowerModeLowPowerRun = LPM_PowerModeLowPowerRun,
    };

    void SetPowerMode(PowerMode_t mode);
};

#endif //PUREPHONE_POWERMGR_HPP
