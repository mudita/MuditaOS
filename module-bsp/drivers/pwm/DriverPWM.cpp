// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DriverPWM.hpp"
#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverPWM.hpp"
#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif

namespace drivers
{
    std::weak_ptr<DriverPWM> DriverPWM::pwmDrivers[static_cast<std::uint32_t>(PWMInstances::COUNT)]
                                                  [static_cast<std::uint32_t>(PWMModules::COUNT)];

    std::shared_ptr<DriverPWM> DriverPWM::Create(drivers::PWMInstances instance,
                                                 drivers::PWMModules module,
                                                 const drivers::DriverPWMParams &params)
    {
        const auto instanceIndex =
            static_cast<std::uint32_t>(instance) - static_cast<std::uint32_t>(PWMInstances::OFFSET);
        const auto moduleIndex = static_cast<std::uint32_t>(module);

        cpp_freertos::CriticalSection::Enter();
        std::shared_ptr<DriverPWM> inst = pwmDrivers[instanceIndex][moduleIndex].lock();

        if (!inst) {
#if defined(TARGET_RT1051)
            inst = std::make_shared<RT1051DriverPWM>(instance, module, params);
#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif
            pwmDrivers[instanceIndex][moduleIndex] = inst;
        }
        else {
            inst->InitNextChannel(params);
        }
        cpp_freertos::CriticalSection::Exit();

        return inst;
    }
} // namespace drivers
