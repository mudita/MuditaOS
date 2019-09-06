/*
 *  @file bsp_lpm.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.09.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "bsp_lpm.hpp"


#if defined(TARGET_RT1051)
#include "board/rt1051/bsp/lpm/RT1051LPM.hpp"
#elif defined(TARGET_Linux)
#include "cellular/linux_cellular.hpp"
#else
#error "Unsupported target"
#endif

namespace bsp{

    std::optional<std::unique_ptr<LowPowerMode>> LowPowerMode::Create() {

        std::unique_ptr<LowPowerMode> inst;

//#if defined(TARGET_RT1051)
//        inst = std::make_unique<bsp::RT1051LPM>();
//#elif defined(TARGET_Linux)
//        inst = std::make_unique<bsp::LinuxCellular>("0");
//#else
//#error "Unsupported target"
//#endif

        return inst;
    }

}
