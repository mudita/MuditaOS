// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DriverUSDHC.hpp"
#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverUSDHC.hpp"
#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif

namespace drivers
{

    std::weak_ptr<DriverUSDHC> DriverUSDHC::singleton[magic_enum::enum_count<USDHCInstances>()];

    std::shared_ptr<DriverUSDHC> DriverUSDHC::Create(std::string name, const drivers::USDHCInstances instance)
    {

        cpp_freertos::CriticalSection::Enter();
        std::shared_ptr<DriverUSDHC> inst = singleton[static_cast<uint32_t>(instance)].lock();

        if (!inst) {
#if defined(TARGET_RT1051)
            inst = std::make_shared<RT1051DriverUSDHC>(std::move(name), instance);
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
