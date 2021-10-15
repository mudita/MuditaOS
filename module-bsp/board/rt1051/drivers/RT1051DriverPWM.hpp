// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/pwm/DriverPWM.hpp"

#include "fsl_common.h"
#include "fsl_pwm.h"

#include <mutex.hpp>

#include <vector>
#include <map>
namespace drivers
{
    class RT1051DriverPWM : public DriverPWM
    {
      public:
        RT1051DriverPWM(PWMInstances inst, PWMModules mod, const DriverPWMParams &params);

        ~RT1051DriverPWM() final;

        void InitNextChannel(const DriverPWMParams &params) final;

        void SetDutyCycle(std::uint8_t dutyCyclePercent, PWMChannel channel) final;

        void Start(PWMChannel channel) final;

        void Stop(PWMChannel channel) final;

        void UpdateClockFrequency(std::uint32_t newFrequency) final;

        enum class PwmState
        {
            Off,
            On,
        };

      private:
        PwmState GetPwmState();

        void SetupPWMChannel(PWMChannel channel);

        void SetupPWMInstance(pwm_signal_param_t *config, unsigned numOfChannels, std::uint32_t clockFrequency);

        void ForceLowOutput(PWMChannel channel);

        void RestorePwmOutput(PWMChannel channel);

        pwm_channels_t getChannelMask(PWMChannel channel);

        bool channelNotEnabled(PWMChannel channel);

        bool otherChannelRunning(PWMChannel channel);

        void stopAll();

        void startAll();

        void restoreDutyCycle();

        PWM_Type *base = nullptr;

        pwm_submodule_t pwmModule = kPWM_Module_0;

        std::uint8_t lastDutyCycle = 0;

        std::array<pwm_signal_param_t, 2> pwmSignalsConfig;

        std::vector<PWMChannel> enabledChannels;
        std::map<PWMChannel, PwmState> pwmChannelState = {
            {PWMChannel::A, PwmState::Off}, {PWMChannel::B, PwmState::Off}, {PWMChannel::X, PwmState::Off}};

        cpp_freertos::MutexStandard frequencyChangeMutex;

        std::uint32_t clockFrequency = 0;
    };

} // namespace drivers
