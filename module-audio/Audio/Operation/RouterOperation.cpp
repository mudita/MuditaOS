/*
 *  @file RouterOperation.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RouterOperation.hpp"


RouterOperation::RouterOperation(const char *file){

    isInitialized = true;
}

int32_t RouterOperation::SetOutputVolume(float vol) {
    profile->SetOutputVolume(vol);
    //audioDevice->OutputVolumeCtrl(vol);
    return static_cast<int32_t >(RetCode::Success);
}

int32_t RouterOperation::SetInputGain(float gain) {
    profile->SetInputGain(gain);
    //audioDevice->InputGainCtrl(gain);
    return static_cast<int32_t >(RetCode::Success);
}