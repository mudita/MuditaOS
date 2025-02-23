// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "bsp/eink_frontlight/eink_frontlight.hpp"
#include "board/BoardDefinitions.hpp"
#include "drivers/pwm/DriverPWM.hpp"
#include <cmath>

namespace bsp::eink_frontlight
{
    namespace
    {
        constexpr auto pwmOutputFrequencyHz = 5000;
        constexpr auto pwmChannel = static_cast<drivers::PWMChannel>(BoardDefinitions::EINK_FRONTLIGHT_PWM_CHANNEL);

        auto gammaFactor          = 2.5f;
        std::shared_ptr<drivers::DriverPWM> pwm;

        float gammaCorrection(BrightnessPercentage brightness)
        {
            constexpr auto minPercent = 0.0f;
            constexpr auto maxPercent = 100.0f;
            std::clamp(brightness, minPercent, maxPercent);
            return (maxPercent * std::pow(brightness / maxPercent, gammaFactor));
        }
    } // namespace

    void init()
    {
        const drivers::DriverPWMParams pwmParams{.channel = pwmChannel, .outputFrequency = pwmOutputFrequencyHz};

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
            pwm->SetDutyCycle(gammaCorrection(brightness), pwmChannel);
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

    void updateClockFrequency()
    {
        pwm->UpdateClockFrequency();
    }
} // namespace bsp::eink_frontlight
