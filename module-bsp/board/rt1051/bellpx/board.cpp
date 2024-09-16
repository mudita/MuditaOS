// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
    void board_configure()
    {
        /* See *
         * https://patchwork.kernel.org/project/linux-arm-kernel/patch/1471885400-9140-1-git-send-email-Anson.Huang@nxp.com/
         */
        CCM->CGPR |= CCM_CGPR_INT_MEM_CLK_LPM_MASK;

        /* ERR050143 */
        IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_GINT_MASK;
    }

    void board_exit(RebootState state)
    {
        switch (state) {
        case RebootState::None:
            break;
        case RebootState::Poweroff:
            power_off();
            break;
        case RebootState::Reboot:
            reset();
            break;
        }

        while (true) {}
    }
} // namespace bsp
