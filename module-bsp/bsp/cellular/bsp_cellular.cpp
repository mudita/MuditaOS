
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
//TODO
#elif defined(TARGET_Linux)

#include "cellular/linux_cellular.hpp"

#else
#error "Unsupported target"
#endif


namespace bsp{

    std::unique_ptr<Cellular> Cellular::Create(const char* term) {

#if defined(TARGET_RT1051)
        //TODO
#elif defined(TARGET_Linux)
        return std::make_unique<bsp::LinuxCellular>(term);
#else
#error "Unsupported target"
#endif

    }

}