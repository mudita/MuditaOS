/*
 *  @file DriverI2C.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "DriverI2C.hpp"


//TODO:M.P this is only unfinished template, I will continue work on this during power manager development
std::shared_ptr<DriverI2C> DriverI2C::Create(const DriverParams &params) {
#if defined(TARGET_RT1051)

#elif defined(TARGET_Linux)
    //inst = std::make_unique<bsp::LinuxAudiocodec>(callback );
#else
                #error "Unsupported target"
#endif
    return nullptr;
}
