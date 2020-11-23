// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/eink_frontlight/eink_frontlight.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "drivers/pwm/DriverPWM.hpp"
#include "fsl_common.h"

using namespace drivers;

static std::shared_ptr<drivers::DriverPWM> pwm;

namespace bsp
{
    namespace eink_frontlight
    {
        constexpr auto PWM_FREQUENCY = 10000; // 10 kHz

        int32_t init()
        {
            pwm = DriverPWM::Create(
                static_cast<PWMInstances>(BoardDefinitions::EINK_FRONTLIGHT_PWM_INSTANCE),
                static_cast<PWMModules>(BoardDefinitions::EINK_FRONTLIGHT_PWM_MODULE),
                DriverPWMParams{.channel   = static_cast<PWMChannel>(BoardDefinitions::EINK_FRONTLIGHT_PWM_CHANNEL),
                                .frequency = PWM_FREQUENCY});

            return kStatus_Success; // Add some checks ?
        }

        void deinit()
        {
            turnOff();
        }

        void setBrightness(uint8_t brightness)
        {
            // Add gamma correction - is it needed - check docu
            pwm->SetDutyCycle(brightness);
        }

        void turnOn()
        {
            pwm->Start();
        }

        void turnOff()
        {
            pwm->Stop();
        }

    } // namespace eink_frontlight
} // namespace bsp
