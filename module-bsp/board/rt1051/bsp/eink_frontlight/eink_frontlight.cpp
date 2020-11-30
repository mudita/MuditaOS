// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/eink_frontlight/eink_frontlight.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "drivers/pwm/DriverPWM.hpp"
#include "fsl_common.h"

namespace bsp::eink_frontlight
{
    namespace
    {
        std::shared_ptr<drivers::DriverPWM> pwm;
        constexpr inline auto PWM_FREQUENCY_HZ = 20000;
        uint8_t bright                         = 0;
    } // namespace

    void init()
    {
        pwm = drivers::DriverPWM::Create(
            static_cast<drivers::PWMInstances>(BoardDefinitions::EINK_FRONTLIGHT_PWM_INSTANCE),
            static_cast<drivers::PWMModules>(BoardDefinitions::EINK_FRONTLIGHT_PWM_MODULE),
            drivers::DriverPWMParams{
                .channel   = static_cast<drivers::PWMChannel>(BoardDefinitions::EINK_FRONTLIGHT_PWM_CHANNEL),
                .frequency = PWM_FREQUENCY_HZ});
    }

    void deinit()
    {
        turnOff();
    }

    void setBrightness(uint8_t brightness)
    {
        // Add gamma correction LUT if needed (from 0-5 scale to 0-100 duty cycle)
        pwm->SetDutyCycle(bright);

        bright += 20;
        if (bright > 100) {
            bright = 0;
        }
    }

    uint8_t GetBrightness()
    {
        // If gamma corection used add recalculation from 0-100 duty cycle to 0-5 scale
        return pwm->GetCurrentDutyCycle();
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
