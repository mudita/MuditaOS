// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp.hpp"
#include "board.h"
#include "drivers/gpio/DriverGPIO.hpp"
#include "board/BoardDefinitions.hpp"
#include "lpm/CpuFreqLPM.hpp"

namespace
{
    using namespace drivers;

    void power_off()
    {
        WDOG1->WCR &= ~WDOG_WCR_WDA_MASK;
        while (1)
            ;
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

        auto cpuFreq = bsp::CpuFreqLPM();
        cpuFreq.SetCpuFrequency(bsp::CpuFreqLPM::CpuClock::CpuClock_Osc_24_Mhz);

        SNVS->LPCR |= SNVS_LPCR_TOP(1); /// Enter SNVS mode
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
