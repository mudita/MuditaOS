// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051LPM.hpp"
#include <board.h>
#include <board/BoardDefinitions.hpp>

namespace bsp
{
    using namespace drivers;

    int32_t RT1051LPM::PowerOff()
    {
        gpio_wakeup =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_WAKEUP_GPIO), DriverGPIOParams{});

        gpio_wakeup->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                                 .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingEdge,
                                                 .defLogic = 0,
                                                 .pin = static_cast<std::uint32_t>(BoardDefinitions::BELL_WAKEUP)});
        gpio_wakeup->ClearPortInterrupts(1 << static_cast<std::uint32_t>(BoardDefinitions::BELL_WAKEUP));
        gpio_wakeup->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP));

        SNVS->LPCR |= SNVS_LPCR_TOP(1); // Enter SNVS mode
        while (1) {                     /* Shutdown */
        }
        return 0;
    }

    void RT1051LPM::EnableDcdcPowerSaveMode()
    {}

    void RT1051LPM::DisableDcdcPowerSaveMode()
    {}
} // namespace bsp
