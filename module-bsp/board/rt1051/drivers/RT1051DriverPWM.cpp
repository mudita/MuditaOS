// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPWM.hpp"
#include "log/log.hpp"
#include "../board.h"

namespace drivers
{
    RT1051DriverPWM::RT1051DriverPWM(const PWMInstances inst, const DriverPWMParams &params) : DriverPWM(inst, params)
    {
        LOG_DEBUG("Init: PWM");
    }

    RT1051DriverPWM::~RT1051DriverPWM()
    {
        LOG_DEBUG("Deinit: PWM");
    }

    void RT1051DriverPWM::SetDutyCycle(uint32_t /*duty_cycle*/)
    {
        LOG_DEBUG("Duty cycle set");
    }

    void RT1051DriverPWM::Start()
    {
        LOG_DEBUG("PWM start");
    }

    void RT1051DriverPWM::Stop()
    {
        LOG_DEBUG("PWM stop");
    }

} // namespace drivers
