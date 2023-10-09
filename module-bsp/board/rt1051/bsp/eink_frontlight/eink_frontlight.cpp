// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/eink_frontlight/eink_frontlight.hpp"
#include "board/BoardDefinitions.hpp"
#include "drivers/pwm/DriverPWM.hpp"
#include <cmath>
#include <log/log.hpp>

namespace bsp::eink_frontlight
{
    namespace
    {
        std::shared_ptr<drivers::DriverPWM> pwm;
        constexpr auto PWM_FREQUENCY_HZ = 20000;
        constexpr auto pwmChannel = static_cast<drivers::PWMChannel>(BoardDefinitions::EINK_FRONTLIGHT_PWM_CHANNEL);
        auto gammaFactor          = 2.5f;

        float gammaCorrection(BrightnessPercentage brightness)
        {
            constexpr auto minPercent = 0.0f;
            constexpr auto maxPercent = 100.0f;
            std::clamp(brightness, minPercent, maxPercent);
            return (maxPercent * std::pow((brightness / maxPercent), gammaFactor));
        }
    } // namespace

    void init()
    {
        const drivers::DriverPWMParams pwmParams = {.channel = pwmChannel, .frequency = PWM_FREQUENCY_HZ};

        pwm = drivers::DriverPWM::Create(
            static_cast<drivers::PWMInstances>(BoardDefinitions::EINK_FRONTLIGHT_PWM_INSTANCE),
            static_cast<drivers::PWMModules>(BoardDefinitions::EINK_FRONTLIGHT_PWM_MODULE),
            pwmParams);
    }

    void deinit()
    {
        turnOff();
    }

    void setBrightness(BrightnessPercentage brightness)
    {
        if (pwm) {
            brightness /= 1.9f; // TODO just some scaling factor to show that it works
            auto dutyCycle = gammaCorrection(brightness);
            /* TODO
             * dirty hack for preventing weird high intensity screen blink after changing duty cycle value
             * from 0 or reenabling channel when operating at duty cycle values <1%. If I understand correctly,
             * DIO5662 can only work with PWMs >=1%. 0.2-0.3% seems to be absolute minimum on my device. */
            if (dutyCycle < 0.2) {
                dutyCycle = 0;
            }
            pwm->SetDutyCycle(dutyCycle, pwmChannel);
            LOG_INFO("Setting duty cycle %f%%", dutyCycle);
        }
    }

    void turnOn()
    {
        pwm->Start(pwmChannel);
    }

    void turnOff()
    {
        pwm->Stop(pwmChannel);
    }

    void setGammaFactor(float gamma)
    {
        gammaFactor = gamma;
    }

    void updateClockFrequency(CpuFrequencyMHz newFrequency)
    {
        pwm->UpdateClockFrequency(newFrequency);
    }
} // namespace bsp::eink_frontlight
