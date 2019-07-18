
/*
 * @file bsp_cellular.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "bsp_cellular.hpp"

#if defined(TARGET_RT1051)
#include "cellular/rt1051_cellular.hpp"
#elif defined(TARGET_Linux)
#include "cellular/linux_cellular.hpp"
#else
#error "Unsupported target"
#endif


namespace bsp{

    std::optional<std::unique_ptr<Cellular>> Cellular::Create(
            [[maybe_unused]] const char* term) {

        std::unique_ptr<Cellular> inst;

#if defined(TARGET_RT1051)
        inst = std::make_unique<bsp::RT1051Cellular>();
#elif defined(TARGET_Linux)
        inst = std::make_unique<bsp::LinuxCellular>(term);
#else
#error "Unsupported target"
#endif

        if(inst->isInitialized){
            return inst;
        }

        return {};
    }

}