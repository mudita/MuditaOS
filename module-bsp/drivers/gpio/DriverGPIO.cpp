/*
 *  @file DriverGPIO.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 14.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "DriverGPIO.hpp"

#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverGPIO.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers {

    std::weak_ptr<DriverGPIO> DriverGPIO::singleton[magic_enum::enum_count<GPIOInstances>()];

    std::shared_ptr<DriverGPIO> DriverGPIO::Create(const drivers::GPIOInstances instance,
                                                 const drivers::DriverGPIOParams &params) {
        {

            cpp_freertos::CriticalSection::Enter();
            std::shared_ptr<DriverGPIO> inst = singleton[magic_enum::enum_integer(instance)].lock();

            if (!inst) {
#if defined(TARGET_RT1051)
                inst = std::make_shared<RT1051DriverGPIO>(instance,params);
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