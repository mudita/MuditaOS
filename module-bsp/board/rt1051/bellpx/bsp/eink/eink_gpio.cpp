// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "drivers/gpio/DriverGPIO.hpp"
#include "board/BoardDefinitions.hpp"

namespace bsp::eink
{
    using namespace drivers;

    namespace
    {
        constexpr auto LogicLow  = 0;
        constexpr auto LogicHigh = 1;
    } // namespace

    void eink_gpio_configure()
    {
        const auto gpio_pwren =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::EINK_BELL_PWR_GPIO), DriverGPIOParams{});

        gpio_pwren->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = LogicHigh,
                                .pin      = static_cast<std::uint32_t>(BoardDefinitions::EINK_BELL_PWR_PIN)});

        const auto gpio =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::EINK_GPIO), DriverGPIOParams{});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                          .defLogic = LogicHigh,
                                          .pin      = static_cast<std::uint32_t>(BoardDefinitions::EINK_CS_PIN)});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                          .defLogic = LogicLow,
                                          .pin      = static_cast<std::uint32_t>(BoardDefinitions::EINK_RESET_PIN)});

        gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                          .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingEdge,
                                          .defLogic = LogicLow,
                                          .pin      = static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN)});

        gpio->ClearPortInterrupts(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
        gpio->DisableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
    }

    void eink_gpio_power_on()
    {
        const auto gpio_pwren =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::EINK_BELL_PWR_GPIO), DriverGPIOParams{});
        gpio_pwren->WritePin(static_cast<std::uint32_t>(BoardDefinitions::EINK_BELL_PWR_PIN), LogicHigh);
    }

    void eink_gpio_power_off()
    {
        const auto gpio_pwren =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::EINK_BELL_PWR_GPIO), DriverGPIOParams{});
        gpio_pwren->WritePin(static_cast<std::uint32_t>(BoardDefinitions::EINK_BELL_PWR_PIN), LogicLow);
    }
} // namespace bsp::eink
