/*
 *  @file DriverSAI.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

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