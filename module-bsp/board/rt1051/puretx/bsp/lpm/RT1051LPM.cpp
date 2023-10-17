// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPM.hpp"
#include <board/BoardDefinitions.hpp>

namespace bsp
{
    using namespace drivers;

    RT1051LPM::RT1051LPM()
    {
        gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::POWER_SWITCH_HOLD_GPIO),
                                    DriverGPIOParams{});
        gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::DCDC_INVERTER_MODE_GPIO),
                                    DriverGPIOParams{});
        gpio_1->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 1,
                                .pin      = static_cast<std::uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<std::uint32_t>(BoardDefinitions::DCDC_INVERTER_MODE_PIN)});

        gpio_1->WritePin(static_cast<std::uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON), 1);
        EnableDcdcPowerSaveMode();
    }

    // Sets the bucks to operate in auto PSM/PWM mode
    // Current threshold for entry to and exit from the PSM mode is set to 100 mA
    void RT1051LPM::EnableDcdcPowerSaveMode()
    {
        gpio_2->WritePin(static_cast<std::uint32_t>(BoardDefinitions::DCDC_INVERTER_MODE_PIN), 0);
    }

    // Forces both bucks to operate in PWM mode
    void RT1051LPM::DisableDcdcPowerSaveMode()
    {
        gpio_2->WritePin(static_cast<std::uint32_t>(BoardDefinitions::DCDC_INVERTER_MODE_PIN), 1);
    }
} // namespace bsp
