// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/vibrator/vibrator.hpp"

#include "board/BoardDefinitions.hpp"
#include <drivers/gpio/DriverGPIO.hpp>
#include <drivers/pwm/DriverPWM.hpp>

namespace bsp
{
    namespace vibrator
    {
        using namespace drivers;

        std::shared_ptr<drivers::DriverPWM> pwm;
        constexpr inline auto PWM_FREQUENCY_HZ = 20000;
        constexpr inline auto levelMultiplier  = 10;
        constexpr auto pwmChannel = static_cast<drivers::PWMChannel>(BoardDefinitions::VIBRATOR_PWM_CHANNEL);

        void init(std::chrono::milliseconds pulse)
        {
            drivers::DriverPWMParams pwmParams;
            pwmParams.channel   = pwmChannel;
            pwmParams.frequency = PWM_FREQUENCY_HZ;

            pwm =
                drivers::DriverPWM::Create(static_cast<drivers::PWMInstances>(BoardDefinitions::VIBRATOR_PWM_INSTANCE),
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

        void updateClockFrequency(CpuFrequencyHz newFrequency)
        {
            pwm->UpdateClockFrequency(static_cast<std::uint32_t>(newFrequency));
        }

        void setVibrationLevel(unsigned int vibrationLevel)
        {
            if (pwm) {
                pwm->SetDutyCycle(vibrationLevel * levelMultiplier, pwmChannel);
            }
        }
    } // namespace vibrator
} // namespace bsp
