// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DriverLPUART.hpp"
#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverLPUART.hpp"
#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif

namespace drivers
{

    std::weak_ptr<DriverLPUART> DriverLPUART::singleton[magic_enum::enum_count<LPUARTInstances>()];

    std::shared_ptr<DriverLPUART> DriverLPUART::Create(std::string name, const drivers::LPUARTInstances instance)
    {
        cpp_freertos::CriticalSection::Enter();
        std::shared_ptr<DriverLPUART> inst = singleton[static_cast<uint32_t>(instance)].lock();

        if (!inst) {
#if defined(TARGET_RT1051)
            inst = std::make_shared<RT1051DriverLPUART>(std::move(name), instance);
#elif defined(TARGET_Linux)
#else
#error "Unsupported target"
#endif

            singleton[static_cast<uint32_t>(instance)] = std::move(inst);
        }

        cpp_freertos::CriticalSection::Exit();

        return inst;
    }

} // namespace drivers
