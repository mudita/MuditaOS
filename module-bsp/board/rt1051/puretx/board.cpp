// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "bsp.hpp"
#include "board.h"
#include "drivers/gpio/DriverGPIO.hpp"
#include <board/BoardDefinitions.hpp>

namespace
{
    void board_shutdown()
    {
        /// No memory allocation here as this specific GPIO was initialized at the startup. We are just grabbing here a
        /// reference to the already existing object.
        using namespace drivers;
        auto gpio_power = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::POWER_SWITCH_HOLD_GPIO),
                                             DriverGPIOParams{});
        gpio_power->WritePin(static_cast<std::uint32_t>(BoardDefinitions::POWER_SWITCH_HOLD_BUTTON), 0);
    }

    void board_reset()
    {
        NVIC_SystemReset();
    }
} // namespace

namespace bsp
{
    void board_configure()
    {
        // no-op for Pure
    }

    void board_exit(RebootState state)
    {
        switch (state) {
        case RebootState::None:
            break;
        case RebootState::Reboot:
        case RebootState::Poweroff:
            board_reset(); // Reset will result in powering off the CPU if USB is disconnected due to hardware design
            break;
        }

        while (true) {}
    }
} // namespace bsp
