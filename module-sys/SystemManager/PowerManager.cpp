/*
 *  @file PowerManager.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 12.09.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "PowerManager.hpp"


namespace sys{

    PowerManager::PowerManager(){
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);
    }

    PowerManager::~PowerManager() {

    }

    int32_t PowerManager::Switch(const sys::PowerManager::Mode mode) {
        switch(mode){
            case Mode::FullSpeed:
                return lowPowerControl->Switch(bsp::LowPowerMode::Mode::FullSpeed);
            case Mode::LowPowerIdle:
            case Mode::LowPowerRun:
            case Mode::Suspend:
                return lowPowerControl->Switch(bsp::LowPowerMode::Mode::LowPowerIdle);
            default:
                return 0;
        }
    }

}