// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/eink_frontlight/eink_frontlight.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "drivers/pwm/DriverPWM.hpp"
#include "fsl_common.h"
#include <cmath>

namespace bsp::eink_frontlight
{
    namespace
    {
        std::shared_ptr<drivers::DriverPWM> pwm;
        constexpr inline auto PWM_FREQUENCY_HZ = 20000;
        constexpr inline float gamma           = 2.5f;

        constexpr inline std::uint8_t gammaCorrection(BrightnessPercentage brightness)
        {
            std::clamp(brightness, static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(100));
            return static_cast<std::uint8_t>(100 * std::pow((brightness / 100.0f), gamma));
        }

    } // namespace

    void init()
    {
        drivers::DriverPWMParams pwmParams;
        pwmParams.channel   = static_cast<drivers::PWMChannel>(BoardDefinitions::EINK_FRONTLIGHT_PWM_CHANNEL);
        pwmParams.frequency = PWM_FREQUENCY_HZ;

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
        pwm->SetDutyCycle(gammaCorrection(brightness));
    }

    void turnOn()
    {
        pwm->Start();
    }

    void turnOff()
    {
        pwm->Stop();
    }

} // namespace bsp::eink_frontlight
