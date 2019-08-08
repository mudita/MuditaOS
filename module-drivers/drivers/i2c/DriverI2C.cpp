/*
 *  @file DriverI2C.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "DriverI2C.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverI2C.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif


//TODO:M.P this is only unfinished template, I will continue work on this during power manager development
std::shared_ptr<DriverI2C> DriverI2C::Create(const DriverParams &params) {

    std::shared_ptr<DriverI2C> inst = instance.lock();

    if(!inst){

    }
#if defined(TARGET_RT1051)

#elif defined(TARGET_Linux)
#else
                #error "Unsupported target"
#endif
    return nullptr;
}
