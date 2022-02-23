// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DriverI2C.hpp"

#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverI2C.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers
{

    std::weak_ptr<DriverI2C> DriverI2C::singleton[static_cast<uint32_t>(I2CInstances::COUNT)];

    std::shared_ptr<DriverI2C> DriverI2C::Create(const drivers::I2CInstances instance,
                                                 const drivers::DriverI2CParams &params)
    {
        {

            cpp_freertos::CriticalSection::Enter();
            std::shared_ptr<DriverI2C> inst = singleton[static_cast<uint32_t>(instance)].lock();

            if (!inst) {
#if defined(TARGET_RT1051)
                inst = std::make_shared<RT1051DriverI2C>(instance, params);
#elif defined(TARGET_Linux)
#else
#error "Unsupported target"
#endif

                singleton[static_cast<uint32_t>(instance)] = inst;
            }

            cpp_freertos::CriticalSection::Exit();

            return inst;
        }
    }

} // namespace drivers