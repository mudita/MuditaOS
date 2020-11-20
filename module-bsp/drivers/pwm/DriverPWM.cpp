// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DriverPWM.hpp"

#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverPWM.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers
{

    std::weak_ptr<DriverPWM> DriverPWM::singleton[static_cast<uint32_t>(PWMInstances ::COUNT)]
                                                 [static_cast<uint32_t>(PWMModules::COUNT)];

    std::shared_ptr<DriverPWM> DriverPWM::Create(const drivers::PWMInstances instance,
                                                 const drivers::PWMModules module,
                                                 const drivers::DriverPWMParams &params)
    {
        {

            cpp_freertos::CriticalSection::Enter();
            std::shared_ptr<DriverPWM> inst =
                singleton[static_cast<uint32_t>(instance)][static_cast<uint32_t>(module)].lock();

            if (!inst) {
#if defined(TARGET_RT1051)
                inst = std::make_shared<RT1051DriverPWM>(instance, module, params);
#elif defined(TARGET_Linux)
#else
#error "Unsupported target"
#endif

                singleton[static_cast<uint32_t>(instance)][static_cast<uint32_t>(module)] = inst;
            }

            cpp_freertos::CriticalSection::Exit();

            return inst;
        }
    }

} // namespace drivers