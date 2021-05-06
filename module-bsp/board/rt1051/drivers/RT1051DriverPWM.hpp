// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        RT1051DriverPWM(PWMInstances inst, PWMModules mod, const DriverPWMParams &params);

        ~RT1051DriverPWM() final;

        void SetDutyCycle(std::uint8_t dutyCyclePercent) final;

        void Start() final;

        void Stop() final;

        void UpdateClockFrequency(std::uint32_t newFrequency) final;

        enum class PwmState
        {
            Off,
            On,
        };

      private:
        PwmState GetPwmState();

        void SetupPWMChannel(PWMChannel channel);

        void SetupPWMInstance(std::uint32_t clockFrequency);

        void ForceLowOutput();

        void RestorePwmOutput();

        PWM_Type *base = nullptr;

        pwm_submodule_t pwmModule = kPWM_Module_0;

        pwm_signal_param_t pwmSignalConfig = {.pwmChannel       = kPWM_PwmB,
                                              .dutyCyclePercent = 0,
                                              .level            = kPWM_HighTrue,
                                              .deadtimeValue    = 0,
                                              .faultState       = kPWM_PwmFaultState0};

        std::uint8_t lastDutyCycle = 0;

        cpp_freertos::MutexStandard frequencyChangeMutex;
    };

} // namespace drivers
