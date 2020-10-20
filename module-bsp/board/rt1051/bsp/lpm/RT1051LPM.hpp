// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051LPM_HPP
#define PUREPHONE_RT1051LPM_HPP

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/gpio/DriverGPIO.hpp"

namespace bsp
{

    class RT1051LPM : public LowPowerMode
    {
      public:
        RT1051LPM();
        int32_t Switch(const Mode mode) override final;
        int32_t PowerOff() override final;
        int32_t Reboot() override final;

      private:
        int32_t EnterLowPowerRun();
        int32_t EnterFullSpeed();
        int32_t EnterLowPowerIdle();
        int32_t EnterSuspend();

        std::shared_ptr<drivers::DriverGPIO> gpio;
    };

} // namespace bsp

#endif // PUREPHONE_RT1051LPM_HPP
