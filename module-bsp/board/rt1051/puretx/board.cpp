// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp.hpp"
#include "board.h"
#include "drivers/gpio/DriverGPIO.hpp"
#include <board/BoardDefinitions.hpp>

namespace
{
    void board_power_off()
    {
        /// No memory allocation here as this specific GPIO was initialized at the startup. We are just grabbing here a
        /// reference to the already existing object.
        using namespace drivers;
        auto gpio_power = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::POWER_SWITCH_HOLD_GPIO),
                                             DriverGPIOParams{});
        gpio_power->WritePin(static_cast<uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON), 0);
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
