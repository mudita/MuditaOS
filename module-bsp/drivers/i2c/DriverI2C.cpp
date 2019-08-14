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

namespace drivers {

    std::shared_ptr<DriverI2C> DriverI2C::Create(const drivers::I2CInstances inst,
                                                 const drivers::DriverI2CParams &params) {
        {
#if defined(TARGET_RT1051)
            return  std::make_shared<RT1051DriverI2C>(inst,params);
#elif defined(TARGET_Linux)
            #else
#error "Unsupported target"
#endif
        }
    }

}