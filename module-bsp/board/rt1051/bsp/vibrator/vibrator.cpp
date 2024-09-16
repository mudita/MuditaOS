// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "bsp/vibrator/vibrator.hpp"

#include "board/BoardDefinitions.hpp"
#include <drivers/pwm/DriverPWM.hpp>

namespace bsp::vibrator
{
    using namespace drivers;

    namespace
    {
        std::shared_ptr<drivers::DriverPWM> pwm;

        constexpr auto pwmOutputFrequencyHz = 5000;
        constexpr auto levelMultiplier      = 10;
        constexpr auto pwmChannel = static_cast<drivers::PWMChannel>(BoardDefinitions::VIBRATOR_PWM_CHANNEL);
    } // namespace

    void init(std::chrono::milliseconds pulse)
    {
        drivers::DriverPWMParams pwmParams;
        pwmParams.channel         = pwmChannel;
        pwmParams.outputFrequency = pwmOutputFrequencyHz;

        pwm = drivers::DriverPWM::Create(static_cast<drivers::PWMInstances>(BoardDefinitions::VIBRATOR_PWM_INSTANCE),
                                         static_cast<drivers::PWMModules>(BoardDefinitions::VIBRATOR_PWM_MODULE),
                                         pwmParams);
        disable();
    }

    void deinit()
    {
        disable();
    }

    void enable()
    {
        pwm->Start(pwmChannel);
    }

    void disable()
    {
        pwm->Stop(pwmChannel);
    }

    void updateClockFrequency()
    {
        if (pwm) {
            pwm->UpdateClockFrequency();
        }
    }

    void setVibrationLevel(unsigned vibrationLevel)
    {
        if (pwm) {
            pwm->SetDutyCycle(vibrationLevel * levelMultiplier, pwmChannel);
        }
    }
} // namespace bsp::vibrator
