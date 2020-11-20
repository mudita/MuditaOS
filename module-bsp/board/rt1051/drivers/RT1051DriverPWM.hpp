// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/pwm/DriverPWM.hpp"

#include "../fsl_drivers/fsl_common.h"
#include "../fsl_drivers/fsl_pwm.h"

#include "mutex.hpp"

namespace drivers
{
    class RT1051DriverPWM : public DriverPWM
    {
      public:
        RT1051DriverPWM(const PWMInstances inst, const PWMModules mod, const DriverPWMParams &params);

        ~RT1051DriverPWM();

        void SetDutyCycle(uint8_t duty_cycle) override final;

        void Start() override final;

        void Stop() override final;

      private:
        void SetupPWMChannel(const PWMChannel channel, const uint32_t frequency);

        PWM_Type *base;

        pwm_submodule_t pwmModule = kPWM_Module_0;

        pwm_signal_param_t pwmSignalConfig = {kPWM_PwmB, 0, kPWM_HighTrue, 0, kPWM_PwmFaultState0};
        // Mode of signal allignement - set to default
        pwm_mode_t pwmMode = kPWM_SignedCenterAligned;

        uint8_t lastDutyCycle = 0;
    };

} // namespace drivers
