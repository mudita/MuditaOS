// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPWM.hpp"
#include "log/log.hpp"
#include "../board.h"

namespace drivers
{
    RT1051DriverPWM::RT1051DriverPWM(const PWMInstances inst, const PWMModules mod, const DriverPWMParams &params)
        : DriverPWM(inst, mod, params)
    {

        pwm_config_t pwmConfig = {0};

        switch (instance) {
        case PWMInstances::PWM_1:
            base = PWM1;
            LOG_DEBUG("Init: PWM1");
            break;
        case PWMInstances::PWM_2:
            base = PWM2;
            LOG_DEBUG("Init: PWM2");
            break;
        case PWMInstances::PWM_3:
            base = PWM3;
            LOG_DEBUG("Init: PWM3");
            break;
        case PWMInstances::PWM_4:
            base = PWM4;
            LOG_DEBUG("Init: PWM4");
            break;
        default:
            LOG_DEBUG("PWM INIT: Invalid instance");
            break;
        }

        switch (module) {
        case PWMModules::MODULE0:
            pwmModule = kPWM_Module_0;
            break;
        case PWMModules::MODULE1:
            pwmModule = kPWM_Module_1;
            break;
        case PWMModules::MODULE2:
            pwmModule = kPWM_Module_2;
            break;
        case PWMModules::MODULE3:
            pwmModule = kPWM_Module_3;
            break;
        default:
            break;
        }

        PWM_GetDefaultConfig(&pwmConfig);
        PWM_Init(base, pwmModule, &pwmConfig);

        SetupPWMChannel(parameters.channel);
    }

    RT1051DriverPWM::~RT1051DriverPWM()
    {
        PWM_Deinit(base, pwmModule);
        LOG_DEBUG("Deinit: PWM");
    }

    void RT1051DriverPWM::SetDutyCycle(uint8_t duty_cycle)
    {
        lastDutyCycle = duty_cycle;
        PWM_UpdatePwmDutycycle(base, pwmModule, pwmSignalConfig.pwmChannel, pwmMode, lastDutyCycle);
        LOG_DEBUG("Duty cycle set");
    }

    void RT1051DriverPWM::Start()
    {
        PWM_UpdatePwmDutycycle(base, pwmModule, pwmSignalConfig.pwmChannel, pwmMode, lastDutyCycle);
        LOG_DEBUG("PWM start");
    }

    void RT1051DriverPWM::Stop()
    {
        uint8_t stoppedDutyCycle = 0;
        PWM_UpdatePwmDutycycle(base, pwmModule, pwmSignalConfig.pwmChannel, pwmMode, stoppedDutyCycle);
        LOG_DEBUG("PWM stop");
    }

    void RT1051DriverPWM::SetupPWMChannel(const PWMChannel &channel)
    {
        uint32_t pwmFrequency   = 10000;
        uint32_t clockFrequency = 100000;

        switch (parameters.channel) {
        case PWMChannel::A:
            pwmSignalConfig.pwmChannel = kPWM_PwmA;
            break;
        case PWMChannel::B:
            pwmSignalConfig.pwmChannel = kPWM_PwmB;
            break;
        case PWMChannel::X:
            pwmSignalConfig.pwmChannel = kPWM_PwmX;
            break;
        default:
            break;
        }

        PWM_SetupPwm(base, pwmModule, &pwmSignalConfig, 1, pwmMode, pwmFrequency, clockFrequency);
    }

} // namespace drivers
