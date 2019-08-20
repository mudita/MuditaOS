/*
 *  @file DriverI2C.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "DriverI2C.hpp"

#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverI2C.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers {

    std::weak_ptr<DriverI2C> DriverI2C::singleton[magic_enum::enum_count<I2CInstances>()];

    std::shared_ptr<DriverI2C> DriverI2C::Create(const drivers::I2CInstances instance,
                                                 const drivers::DriverI2CParams &params) {
        {

            cpp_freertos::CriticalSection::Enter();
            std::shared_ptr<DriverI2C> inst = singleton[magic_enum::enum_integer(instance)].lock();

            if (!inst) {
#if defined(TARGET_RT1051)
                inst = std::make_shared<RT1051DriverI2C>(instance,params);
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