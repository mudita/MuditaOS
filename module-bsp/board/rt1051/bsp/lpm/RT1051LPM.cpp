/*
 *  @file RT1051LPM.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.09.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051LPM.hpp"

#include "board.h"
#include "fsl_iomuxc.h"
#include "fsl_dcdc.h"
#include "fsl_gpc.h"
#include "log/log.hpp"


namespace bsp{

    int32_t RT1051LPM::Switch(const bsp::LowPowerMode::Mode mode) {
        switch (mode){
            case Mode ::FullSpeed:
                return EnterFullSpeed();
            case Mode ::LowPowerRun:
                return EnterLowPowerRun();
            case Mode ::LowPowerIdle:
                return EnterLowPowerIdle();
            case Mode ::Suspend:
                return EnterSuspend();
        }
    }


    int32_t RT1051LPM::EnterFullSpeed() {
        LPM_EnterFullSpeed();
        return 0;
    }

    int32_t RT1051LPM::EnterLowPowerRun() {
        LPM_EnterLowPowerRun();
        return 0;
    }

    int32_t RT1051LPM::EnterLowPowerIdle() {
        LPM_EnterLowPowerIdle();
        return 0;
    }

    int32_t RT1051LPM::EnterSuspend() {
        return 0;
    }
}