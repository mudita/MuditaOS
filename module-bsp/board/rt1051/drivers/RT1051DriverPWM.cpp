// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPWM.hpp"
#include "RT1051DriverPWMhelper.hpp"
#include <log/log.hpp>
#include "../board.h"
#include <algorithm>

namespace drivers
{
    namespace
    {
        constexpr pwm_mode_t pwmMode = kPWM_SignedCenterAligned;
    }

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
        Start(parameters.channel);
    }

    void RT1051DriverPWM::InitNextChannel(const DriverPWMParams &params)
    {
        LOG_INFO("Channel from this PWM module already enabled - PWM will use parameters from initial config");
        SetupPWMChannel(params.channel);
        Start(params.channel);
    }

    RT1051DriverPWM::~RT1051DriverPWM()
    {
        PWM_Deinit(base, pwmModule);
        LOG_DEBUG("Deinit: PWM");
    }

    void RT1051DriverPWM::SetDutyCycle(std::uint8_t dutyCyclePercent, PWMChannel channel)
    {
        std::uint8_t dutyCycle =
            std::clamp(dutyCyclePercent, static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(100));

        auto pwmChannel = getChannelMask(channel);
        for (unsigned i = 0; i < enabledChannels.size(); ++i) {
            if (pwmSignalsConfig[i].pwmChannel == pwmChannel) {
                pwmSignalsConfig[i].dutyCyclePercent = dutyCycle;
            }
        }

        PWM_UpdatePwmDutycycle(base, pwmModule, pwmChannel, pwmMode, dutyCycle);
        PWM_SetPwmLdok(base, 1 << pwmModule, true);
    }

    void RT1051DriverPWM::Start(PWMChannel channel)
    {
        pwmChannelState[channel] = RT1051DriverPWM::PwmState::On;
        RestorePwmOutput(channel);
        if (not otherChannelRunning(channel)) {
            PWM_StartTimer(base, 1 << pwmModule);
        }
    }

    void RT1051DriverPWM::Stop(PWMChannel channel)
    {
        if (not otherChannelRunning(channel)) {
            PWM_StopTimer(base, 1 << pwmModule);
        }
        ForceLowOutput(channel);
        pwmChannelState[channel] = RT1051DriverPWM::PwmState::Off;
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
        if (channelNotEnabled(channel)) {
            auto currentInstance = enabledChannels.size();
            auto pwmChannel      = getChannelMask(channel);

            pwmSignalsConfig[currentInstance].pwmChannel       = pwmChannel;
            pwmSignalsConfig[currentInstance].dutyCyclePercent = 0;
            pwmSignalsConfig[currentInstance].level            = kPWM_HighTrue;
            pwmSignalsConfig[currentInstance].deadtimeValue    = 0;
            pwmSignalsConfig[currentInstance].faultState       = kPWM_PwmFaultState0;

            clockFrequency = DEFAULT_SYSTEM_CLOCK;
            SetupPWMInstance(&pwmSignalsConfig[currentInstance], 1, clockFrequency);

            PWM_SetupFaultDisableMap(base, pwmModule, pwmChannel, kPWM_faultchannel_0, 0);

            // Force logic config
            PWM_SetupSwCtrlOut(base, pwmModule, pwmChannel, false);
            base->SM[pwmModule].CTRL2 |= PWM_CTRL2_FRCEN(1U);

            enabledChannels.push_back(channel);
        }
    }

    void RT1051DriverPWM::SetupPWMInstance(pwm_signal_param_t *config,
                                           unsigned numOfChannels,
                                           std::uint32_t _clockFrequency)
    {
        pwm_mode_t pwmMode = kPWM_SignedCenterAligned;
        PWM_SetupPwm(base, pwmModule, config, numOfChannels, pwmMode, parameters.frequency, _clockFrequency);
    }

    void RT1051DriverPWM::ForceLowOutput(PWMChannel channel)
    {
        PWM_SetupForceSignal(base, pwmModule, getChannelMask(channel), kPWM_SoftwareControl);
        base->SM[pwmModule].CTRL2 |= PWM_CTRL2_FORCE(1U);
    }

    void RT1051DriverPWM::RestorePwmOutput(PWMChannel channel)
    {
        PWM_SetupForceSignal(base, pwmModule, getChannelMask(channel), kPWM_UsePwm);
        base->SM[pwmModule].CTRL2 |= PWM_CTRL2_FORCE(1U);
    }

    void RT1051DriverPWM::UpdateClockFrequency(std::uint32_t newFrequency)
    {
        cpp_freertos::LockGuard lock(frequencyChangeMutex);

        if (clockFrequency != newFrequency) {
            SetupPWMInstance(pwmSignalsConfig.data(), enabledChannels.size(), newFrequency);
            if (GetPwmState() == PwmState::On) {
                stopAll();
                restoreDutyCycle();
                startAll();
            }
            clockFrequency = newFrequency;
        }
    }

    pwm_channels_t RT1051DriverPWM::getChannelMask(PWMChannel channel)
    {
        switch (channel) {
        case PWMChannel::A:
            return kPWM_PwmA;
        case PWMChannel::B:
            return kPWM_PwmB;
        case PWMChannel::X:
            return kPWM_PwmX;
        }
        LOG_FATAL("No mask for given PWM channel!");
        return kPWM_PwmB;
    }

    bool RT1051DriverPWM::channelNotEnabled(PWMChannel channel)
    {
        for (const auto &chan : enabledChannels) {
            if (chan == channel || chan == PWMChannel::X) {
                LOG_FATAL("PWM Channel already enabled!");
                return false;
            }
        }
        return true;
    }

    bool RT1051DriverPWM::otherChannelRunning(PWMChannel channel)
    {
        for (const auto &[otherChannel, state] : pwmChannelState) {
            if (channel != otherChannel && state == RT1051DriverPWM::PwmState::On) {
                return true;
            }
        }
        return false;
    }

    void RT1051DriverPWM::stopAll()
    {
        PWM_StopTimer(base, 1 << pwmModule);
        for (const auto &[channel, state] : pwmChannelState) {
            if (state == RT1051DriverPWM::PwmState::On) {
                ForceLowOutput(channel);
            }
        }
    }

    void RT1051DriverPWM::startAll()
    {
        for (const auto &[channel, state] : pwmChannelState) {
            if (state == RT1051DriverPWM::PwmState::On) {
                RestorePwmOutput(channel);
            }
        }
        PWM_StartTimer(base, 1 << pwmModule);
    }

    void RT1051DriverPWM::restoreDutyCycle()
    {
        for (unsigned i = 0; i < enabledChannels.size(); ++i) {
            PWM_UpdatePwmDutycycle(
                base, pwmModule, pwmSignalsConfig[i].pwmChannel, pwmMode, pwmSignalsConfig[i].dutyCyclePercent);
            PWM_SetPwmLdok(base, 1 << pwmModule, true);
        }
    }

} // namespace drivers
