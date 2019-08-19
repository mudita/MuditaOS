/*
 *  @file DriverGPIO.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 14.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "DriverGPIO.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverGPIO.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers {

    std::shared_ptr<DriverGPIO> DriverGPIO::Create(const drivers::GPIOInstances inst,
                                                 const drivers::DriverGPIOParams &params) {
        {
#if defined(TARGET_RT1051)
            return  std::make_shared<RT1051DriverGPIO>(inst,params);
#elif defined(TARGET_Linux)
            #else
#error "Unsupported target"
#endif
        }
    }

}