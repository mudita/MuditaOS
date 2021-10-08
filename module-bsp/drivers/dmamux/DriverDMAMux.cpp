// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DriverDMAMux.hpp"
#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverDMAMux.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers
{

    std::weak_ptr<DriverDMAMux> DriverDMAMux::singleton[static_cast<uint32_t>(DMAMuxInstances ::COUNT)];

    std::shared_ptr<DriverDMAMux> DriverDMAMux::Create(const drivers::DMAMuxInstances instance,
                                                       const drivers::DriverDMAMuxParams &params)
    {
        {

            cpp_freertos::CriticalSection::Enter();
            std::shared_ptr<DriverDMAMux> inst = singleton[static_cast<uint32_t>(instance)].lock();

            if (!inst) {
#if defined(TARGET_RT1051)
                inst = std::make_shared<RT1051DriverDMAMux>(instance, params);
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