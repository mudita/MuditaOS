/*
 *  @file RT1051LPM.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.09.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RT1051LPM_HPP
#define PUREPHONE_RT1051LPM_HPP

#include "bsp/lpm/bsp_lpm.hpp"

namespace bsp {

    class RT1051LPM : public LowPowerMode {
    public:
        int32_t Switch(const Mode mode) override final;
    private:
        int32_t EnterLowPowerRun();
        int32_t EnterFullSpeed();
        int32_t EnterLowPowerIdle();
        int32_t EnterSuspend();
    };

}


#endif //PUREPHONE_RT1051LPM_HPP
