// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DriverSEMC.hpp"
#include "critical.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverSEMC.hpp"
#elif defined(TARGET_Linux)

#else
#error "Unsupported target"
#endif

namespace drivers
{

    std::weak_ptr<DriverSEMC> DriverSEMC::singleton;

    std::shared_ptr<DriverSEMC> DriverSEMC::Create(std::string name)
    {

        cpp_freertos::CriticalSection::Enter();
        std::shared_ptr<DriverSEMC> inst = singleton.lock();

        if (!inst) {
#if defined(TARGET_RT1051)
            inst = std::make_shared<RT1051DriverSEMC>(std::move(name));
#elif defined(TARGET_Linux)
#else
#error "Unsupported target"
#endif

            singleton = std::move(inst);
        }

        cpp_freertos::CriticalSection::Exit();

        return inst;
    }

} // namespace drivers
