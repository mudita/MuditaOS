// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp.hpp"
#include "board.h"
#include "drivers/gpio/DriverGPIO.hpp"
#include "board/BoardDefinitions.hpp"

namespace
{
    using namespace drivers;

    void board_power_off()
    {
        /// No memory allocation here as this specific GPIO was initialized at the startup. We are just grabbing here a
        /// reference to the already existing object
        auto gpio_wakeup =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_WAKEUP_GPIO), DriverGPIOParams{});

        gpio_wakeup->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                                 .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingEdge,
                                                 .defLogic = 0,
                                                 .pin = static_cast<std::uint32_t>(BoardDefinitions::BELL_WAKEUP)});
        gpio_wakeup->ClearPortInterrupts(1 << static_cast<std::uint32_t>(BoardDefinitions::BELL_WAKEUP));
        gpio_wakeup->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));

        SNVS->LPCR |= SNVS_LPCR_TOP(1); /// Enter SNVS mode
    }

    void board_reset()
    {
        NVIC_SystemReset();
    }
} // namespace

namespace bsp
{
    void board_exit(rebootState state)
    {
        switch (state) {
        case rebootState::none:
            break;
        case rebootState::poweroff:
            board_power_off();
            break;
        case rebootState::reboot:
            board_reset();
            break;
        }
        while (true) {}
    }
} // namespace bsp
