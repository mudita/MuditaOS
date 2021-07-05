// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPWM.hpp"
#include "RT1051DriverPWMhelper.hpp"
#include <log.hpp>
#include "../board.h"
#include <algorithm>

namespace drivers
{
    RT1051DriverPWM::RT1051DriverPWM(PWMInstances inst, PWMModules mod, const DriverPWMParams &params)
        : DriverPWM(inst, mod, params)
    {

        pwm_config_t pwmConfig = {};

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
            break;
        }

        switch (module) {
        case PWMModules::MODULE0:
            pwmModule = kPWM_Module_0;
            LOG_DEBUG("Init: PWM module 0");
            break;
        case PWMModules::MODULE1:
            pwmModule = kPWM_Module_1;
            LOG_DEBUG("Init: PWM module 1");
            break;
        case PWMModules::MODULE2:
            pwmModule = kPWM_Module_2;
            LOG_DEBUG("Init: PWM module 2");
            break;
        case PWMModules::MODULE3:
            pwmModule = kPWM_Module_3;
            LOG_DEBUG("Init: PWM module 3");
            break;
        default:
            break;
        }

        PWM_GetDefaultConfig(&pwmConfig);
        PWM_Init(base, pwmModule, &pwmConfig);

        SetupPWMChannel(parameters.channel);
        Start();
    }

    RT1051DriverPWM::~RT1051DriverPWM()
    {
        PWM_Deinit(base, pwmModule);
        LOG_DEBUG("Deinit: PWM");
    }

    void RT1051DriverPWM::SetDutyCycle(std::uint8_t dutyCyclePercent)
    {
        pwm_mode_t pwmMode = kPWM_SignedCenterAligned;

        std::uint8_t dutyCycle =
            std::clamp(dutyCyclePercent, static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(100));
        pwmSignalConfig.dutyCyclePercent = dutyCycle;
        PWM_UpdatePwmDutycycle(base, pwmModule, pwmSignalConfig.pwmChannel, pwmMode, dutyCycle);
        PWM_SetPwmLdok(base, 1 << pwmModule, true);
    }

    void RT1051DriverPWM::Start()
    {
        RestorePwmOutput();
        PWM_StartTimer(base, 1 << pwmModule);
    }

    void RT1051DriverPWM::Stop()
    {
        PWM_StopTimer(base, 1 << pwmModule);
        ForceLowOutput();
    }

    RT1051DriverPWM::PwmState RT1051DriverPWM::GetPwmState()
    {
        if (PWM_GetPwmGeneratorState(base, 1 << pwmModule)) {
            return PwmState::On;
        }
        else {
            return PwmState::Off;
        }
    }

    void RT1051DriverPWM::SetupPWMChannel(PWMChannel channel)
    {
        switch (parameters.channel) {
        case PWMChannel::A:
            pwmSignalConfig.pwmChannel = kPWM_PwmA;
            LOG_DEBUG("Init: PWM channel A");
            break;
        case PWMChannel::B:
            pwmSignalConfig.pwmChannel = kPWM_PwmB;
            LOG_DEBUG("Init: PWM channel B");
            break;
        case PWMChannel::X:
            pwmSignalConfig.pwmChannel = kPWM_PwmX;
            LOG_DEBUG("Init: PWM channel X");
            break;
        }

        // Currently connected to IPbus clock
        const auto clockSource = CLOCK_GetFreq(kCLOCK_IpgClk);
        SetupPWMInstance(clockSource);

        PWM_SetupFaultDisableMap(base, pwmModule, pwmSignalConfig.pwmChannel, kPWM_faultchannel_0, 0);

        // Force logic config
        PWM_SetupSwCtrlOut(base, pwmModule, pwmSignalConfig.pwmChannel, false);
        base->SM[pwmModule].CTRL2 |= PWM_CTRL2_FRCEN(1U);
    }

    void RT1051DriverPWM::SetupPWMInstance(std::uint32_t clockFrequency)
    {
        pwm_mode_t pwmMode = kPWM_SignedCenterAligned;
        PWM_SetupPwm(base, pwmModule, &pwmSignalConfig, 1, pwmMode, parameters.frequency, clockFrequency);
    }

    void RT1051DriverPWM::ForceLowOutput()
    {
        PWM_SetupForceSignal(base, pwmModule, pwmSignalConfig.pwmChannel, kPWM_SoftwareControl);
        base->SM[pwmModule].CTRL2 |= PWM_CTRL2_FORCE(1U);
    }

    void RT1051DriverPWM::RestorePwmOutput()
    {
        PWM_SetupForceSignal(base, pwmModule, pwmSignalConfig.pwmChannel, kPWM_UsePwm);
        base->SM[pwmModule].CTRL2 |= PWM_CTRL2_FORCE(1U);
    }

    void RT1051DriverPWM::UpdateClockFrequency(std::uint32_t newFrequency)
    {
        cpp_freertos::LockGuard lock(frequencyChangeMutex);

        SetupPWMInstance(newFrequency);
        if (GetPwmState() == PwmState::On) {
            Stop();
            SetDutyCycle(pwmSignalConfig.dutyCyclePercent);
            Start();
        }
    }

} // namespace drivers
