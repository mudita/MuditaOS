/*
 *  @file RT1051LPM.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 04.09.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051LPM.hpp"

#include "board.h"
#include "log/log.hpp"
#include "bsp/BoardDefinitions.hpp"


namespace bsp{

    using namespace drivers;

    RT1051LPM::RT1051LPM(){
        gpio = DriverGPIO::Create(static_cast<GPIOInstances >(BoardDefinitions::POWER_SWITCH_HOLD_GPIO),
                                  DriverGPIOParams{});

        gpio->ConfPin(DriverGPIOPinParams{.dir=DriverGPIOPinParams::Direction::Output,
                .irqMode=DriverGPIOPinParams::InterruptMode::NoIntmode,
                .defLogic = 1,
                .pin = static_cast<uint32_t >(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON)});

    }

    int32_t RT1051LPM::Switch(const bsp::LowPowerMode::Mode mode) {
        currentMode = mode;
        switch (mode){
            case Mode ::FullSpeed:
                return EnterFullSpeed();
            case Mode ::LowPowerRun:
                return EnterLowPowerRun();
            case Mode ::LowPowerIdle:
                return EnterLowPowerIdle();
            case Mode ::Suspend:
                return EnterSuspend();
        }
    }

    int32_t RT1051LPM::PowerOff() {
        gpio->WritePin(static_cast<uint32_t >(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON),0);
        return 0;
    }


    int32_t RT1051LPM::EnterFullSpeed() {
        LPM_EnterFullSpeed();
        return 0;
    }

    int32_t RT1051LPM::EnterLowPowerRun() {
        LPM_EnterLowPowerRun();
        return 0;
    }

    int32_t RT1051LPM::EnterLowPowerIdle() {
        LPM_EnterLowPowerIdle();
        return 0;
    }

    int32_t RT1051LPM::EnterSuspend() {
        return 0;
    }
}