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
#include "drivers/gpio/DriverGPIO.hpp"

namespace bsp {

    class RT1051LPM : public LowPowerMode {
    public:
        RT1051LPM();
        int32_t Switch(const Mode mode) override final;
        int32_t PowerOff() override final;
    private:
        int32_t EnterLowPowerRun();
        int32_t EnterFullSpeed();
        int32_t EnterLowPowerIdle();
        int32_t EnterSuspend();

        std::shared_ptr<drivers::DriverGPIO> gpio;
    };



}


#endif //PUREPHONE_RT1051LPM_HPP
