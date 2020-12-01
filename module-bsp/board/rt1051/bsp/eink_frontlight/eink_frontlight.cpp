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
        constexpr inline auto PWM_FREQUENCY_HZ       = 20000;
        constexpr inline auto EINK_BRIGHTNESS_LEVELS = 5;

        // 0 to N points mapping to 0-100 duty cycle with gamma correction
        template <unsigned N> struct GammaCorrectionLUT
        {
            static_assert(N > 0, "Number of elements should be more than 0");
            constexpr GammaCorrectionLUT() : values()
            {
                constexpr float gamma = 2.5;
                constexpr float scale = 100 / N;
                for (auto i = 0; i <= N; ++i) {
                    values[i] = static_cast<std::uint8_t>(100 * std::pow(((i * scale) / 100), gamma));
                }
            }
            std::array<std::uint8_t, N + 1> values;
        };
        GammaCorrectionLUT<EINK_BRIGHTNESS_LEVELS> gammaCorrection;

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

    void setBrightness(std::uint8_t brightness)
    {
        std::clamp(
            brightness, static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(gammaCorrection.values.size() - 1));
        pwm->SetDutyCycle(gammaCorrection.values[brightness]);
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
