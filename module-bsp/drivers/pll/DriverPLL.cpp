/*
 *  @file DriverPLL.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "DriverPLL.hpp"
#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverPLL.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers {

    std::weak_ptr<DriverPLL> DriverPLL::singleton[magic_enum::enum_count<PLLInstances>()];

    std::shared_ptr<DriverPLL> DriverPLL::Create(const drivers::PLLInstances instance,
                                                 const drivers::DriverPLLParams &params) {
        {

            cpp_freertos::CriticalSection::Enter();
            std::shared_ptr<DriverPLL> inst = singleton[magic_enum::enum_integer(instance)].lock();

            if (!inst) {
#if defined(TARGET_RT1051)
                inst = std::make_shared<RT1051DriverPLL>(instance,params);
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