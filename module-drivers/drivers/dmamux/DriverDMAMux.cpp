/*
 *  @file DriverDMAMux.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "DriverDMAMux.hpp"
#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverDMAMux.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers {

    std::shared_ptr<DriverDMAMux> DriverDMAMux::Create(const drivers::DMAMuxInstances inst,
                                                 const drivers::DriverDMAMuxParams &params) {
        {
#if defined(TARGET_RT1051)
            return  std::make_shared<RT1051DriverDMAMux>(inst,params);
#elif defined(TARGET_Linux)
            #else
#error "Unsupported target"
#endif
        }
    }

}