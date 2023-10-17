// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp.hpp"
#include "board.h"
#include "drivers/gpio/DriverGPIO.hpp"
#include "board/BoardDefinitions.hpp"
#include "bsp/lpm/RT1051LPM.hpp"
#include "log/log.hpp"

namespace
{
    using namespace drivers;

    constexpr auto powerOffFrequencyLevel = bsp::CpuFrequencyMHz::Level_0;
    constexpr auto powerOffFrequencyInHz  = static_cast<std::uint32_t>(powerOffFrequencyLevel) * 1'000'000;

    void power_off()
    {
        // No memory allocation here as this specific GPIO was initialized at the startup. We are just grabbing here a
        // reference to the already existing object
        auto gpio_wakeup =
            DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_WAKEUP_GPIO), DriverGPIOParams{});

        gpio_wakeup->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                                 .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingEdge,
                                                 .defLogic = 0,
                                                 .pin = static_cast<std::uint32_t>(BoardDefinitions::BELL_WAKEUP)});
        gpio_wakeup->ClearPortInterrupts(1 << static_cast<std::uint32_t>(BoardDefinitions::BELL_WAKEUP));
        gpio_wakeup->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BELL_WAKEUP));

        auto cpu = bsp::RT1051LPM();
        cpu.SetCpuFrequency(powerOffFrequencyLevel);

        // If the CPU frequency is wrong just skip the enter to the SNVS mode
        // and wait for the watchdog
        const auto frequency = cpu.GetCpuFrequency();
        if (frequency == powerOffFrequencyInHz) {
            SNVS->LPCR |= SNVS_LPCR_TOP(1); // Enter SNVS mode
        }
        else {
            LOG_FATAL("Can't enter into SNVS mode due to wrong CPU frequency! Current frequency: %ld", frequency);
        }
    }

    void reset()
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
            power_off();
            break;
        case rebootState::reboot:
            reset();
            break;
        }
        while (true) {}
    }
} // namespace bsp
