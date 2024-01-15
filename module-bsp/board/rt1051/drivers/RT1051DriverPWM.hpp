// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/pwm/DriverPWM.hpp"

#include "fsl_common.h"
#include "fsl_pwm.h"

#include <mutex.hpp>

#include <vector>
#include <map>
#include <array>

namespace drivers
{
    class RT1051DriverPWM final : public DriverPWM
    {
      public:
        RT1051DriverPWM(PWMInstances inst, PWMModules mod, const DriverPWMParams &params);

        ~RT1051DriverPWM();

        void InitNextChannel(const DriverPWMParams &params);

        void SetDutyCycle(std::uint8_t dutyCyclePercent, PWMChannel channel);

        void Start(PWMChannel channel);

        void Stop(PWMChannel channel);

        void UpdateClockFrequency();

        enum class PwmState
        {
            Off,
            On
        };

      private:
        PwmState GetPwmState();

        void SetupPWMChannel(PWMChannel channel);

        void SetupPWMInstance(pwm_signal_param_t *config, unsigned numOfChannels, std::uint32_t moduleClockFrequency);

        void ForceLowOutput(PWMChannel channel);

        void RestorePwmOutput(PWMChannel channel);

        pwm_channels_t getChannelMask(PWMChannel channel);

        bool channelEnabled(PWMChannel channel);

        bool otherChannelRunning(PWMChannel channel);

        void stopAll();

        void startAll();

        void restoreDutyCycle();

        PWM_Type *base = nullptr;

        pwm_submodule_t pwmModule = kPWM_Module_0;

        std::array<pwm_signal_param_t, 2> pwmSignalsConfig;

        std::vector<PWMChannel> enabledChannels;
        std::map<PWMChannel, PwmState> pwmChannelState = {
            {PWMChannel::A, PwmState::Off}, {PWMChannel::B, PwmState::Off}, {PWMChannel::X, PwmState::Off}};

        cpp_freertos::MutexStandard frequencyChangeMutex;

        std::uint32_t pwmModuleClockFrequency = 0;
    };

} // namespace drivers
