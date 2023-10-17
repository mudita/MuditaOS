// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DriverSAI.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/drivers/RT1051DriverSAI.hpp"
#elif defined(TARGET_Linux)

//#include ""
#else
#error "Unsupported target"
#endif

namespace drivers
{

    std::shared_ptr<DriverSAI> DriverSAI::Create(const drivers::SAIInstances inst,
                                                 const drivers::DriverSAIParams &params)
    {
        {
#if defined(TARGET_RT1051)
            return std::make_shared<RT1051DriverSAI>(inst, params);
#elif defined(TARGET_Linux)
#else
#error "Unsupported target"
#endif
        }
    }

} // namespace drivers