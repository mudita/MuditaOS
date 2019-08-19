/*
 *  @file DriverDMA.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "DriverDMA.hpp"
#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverDMA.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers {

    std::weak_ptr<DriverDMA> DriverDMA::singleton[magic_enum::enum_count<DMAInstances>()];

    std::shared_ptr<DriverDMA> DriverDMA::Create(const drivers::DMAInstances instance,
                                                       const drivers::DriverDMAParams &params) {
        {

            cpp_freertos::CriticalSection::Enter();
            std::shared_ptr<DriverDMA> inst = singleton[magic_enum::enum_integer(instance)].lock();

            if (!inst) {
#if defined(TARGET_RT1051)
                inst = std::make_shared<RT1051DriverDMA>(instance,params);
#elif defined(TARGET_Linux)
                #else
#error "Unsupported target"
#endif

                singleton[magic_enum::enum_integer(instance)] = inst;
            }

            cpp_freertos::CriticalSection::Exit();

            return inst;

        }
    }

}
