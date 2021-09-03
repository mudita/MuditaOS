// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "drivers/gpio/DriverGPIO.hpp"
#include "board/BoardDefinitions.hpp"

namespace bsp::eink
{

    void eink_configure_gpio_pins()
    {
        using namespace drivers;

        auto gpio_pwren =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::EINK_BELL_PWR_GPIO), DriverGPIOParams{});

        gpio_pwren->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                                .defLogic = 1,
                                                .pin = static_cast<uint32_t>(BoardDefinitions::EINK_BELL_PWR_PIN)});

        auto gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::EINK_GPIO), DriverGPIOParams{});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                          .defLogic = 1,
                                          .pin      = static_cast<uint32_t>(BoardDefinitions::EINK_CS_PIN)});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                          .defLogic = 0,
                                          .pin      = static_cast<uint32_t>(BoardDefinitions::EINK_RESET_PIN)});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingEdge,
                                          .defLogic = 0,
                                          .pin      = static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN)});

        gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
    }
} // namespace bsp::eink
