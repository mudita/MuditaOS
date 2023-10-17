// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "board/irq_gpio.hpp"

#include "board.h"
#include "FreeRTOS.h"
#include <fsl_common.h>
#include <fsl_qtmr.h>
#include <fsl_gpc.h>
#include <fsl_pmu.h>
#include <fsl_rtwdog.h>

#include "board/rt1051/bsp/eink/bsp_eink.h"
#include <hal/key_input/KeyInput.hpp>
#include <hal/battery_charger/BatteryChargerIRQ.hpp>
#include "board/BoardDefinitions.hpp"

namespace bsp
{
    void irq_gpio_Init()
    {
        DisableIRQ(GPIO1_Combined_0_15_IRQn);
        DisableIRQ(GPIO1_Combined_16_31_IRQn);
        DisableIRQ(GPIO2_Combined_0_15_IRQn);
        DisableIRQ(GPIO2_Combined_16_31_IRQn);
        DisableIRQ(GPIO3_Combined_16_31_IRQn);
        DisableIRQ(GPIO5_Combined_0_15_IRQn);
        DisableIRQ(TMR3_IRQn);
        DisableIRQ(RTWDOG_IRQn);
        DisableIRQ(ANATOP_EVENT0_IRQn);

        GPIO_PortDisableInterrupts(GPIO1, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO2, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO3, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO5, UINT32_MAX);

        // Clear all IRQs
        GPIO_PortClearInterruptFlags(GPIO1, UINT32_MAX);
        GPIO_PortClearInterruptFlags(GPIO2, UINT32_MAX);
        GPIO_PortClearInterruptFlags(GPIO3, UINT32_MAX);
        GPIO_PortClearInterruptFlags(GPIO5, UINT32_MAX);
        QTMR_ClearStatusFlags(TMR3,
                              kQTMR_Channel_0,
                              kQTMR_CompareFlag | kQTMR_Compare1Flag | kQTMR_Compare2Flag | kQTMR_OverflowFlag |
                                  kQTMR_EdgeFlag);

        EnableIRQ(GPIO1_Combined_0_15_IRQn);
        NVIC_SetPriority(GPIO1_Combined_0_15_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);

        EnableIRQ(GPIO1_Combined_16_31_IRQn);
        NVIC_SetPriority(GPIO1_Combined_16_31_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);

        EnableIRQ(GPIO2_Combined_0_15_IRQn);
        NVIC_SetPriority(GPIO2_Combined_0_15_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);

        EnableIRQ(GPIO2_Combined_16_31_IRQn);
        NVIC_SetPriority(GPIO2_Combined_16_31_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);

        EnableIRQ(GPIO3_Combined_16_31_IRQn);
        NVIC_SetPriority(GPIO3_Combined_16_31_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);

        NVIC_ClearPendingIRQ(GPIO5_Combined_0_15_IRQn);
        EnableIRQ(GPIO5_Combined_0_15_IRQn);
        GPC_EnableIRQ(GPC, GPIO5_Combined_0_15_IRQn);

        EnableIRQ(TMR3_IRQn);
        NVIC_SetPriority(TMR3_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);

        NVIC_ClearPendingIRQ(RTWDOG_IRQn);
        NVIC_EnableIRQ(RTWDOG_IRQn);

        // Enable PMU brownout interrupt
        NVIC_ClearPendingIRQ(ANATOP_EVENT0_IRQn);
        NVIC_EnableIRQ(ANATOP_EVENT0_IRQn);
    }

    extern "C"
    {
        void GPIO1_Combined_0_15_IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO1);

            if (irq_mask & (1 << static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGOK_PIN))) {
                xHigherPriorityTaskWoken |= hal::battery::charger_irq();
            }
            if (irq_mask & (1 << static_cast<uint32_t>(BoardDefinitions::BELL_FUELGAUGE_ALRT_PIN))) {
                xHigherPriorityTaskWoken |= hal::battery::fuel_gauge_irq();
            }

            // Clear all IRQs
            GPIO_PortClearInterruptFlags(GPIO1, irq_mask);

            // Switch context if necessary
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }

        void GPIO1_Combined_16_31_IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO1);

            // Clear all IRQs
            GPIO_PortClearInterruptFlags(GPIO1, irq_mask);

            // Switch context if necessary
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }

        void GPIO2_Combined_0_15_IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO2);

            // Clear all IRQs
            GPIO_PortClearInterruptFlags(GPIO2, irq_mask);

            // Switch context if necessary
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }

        void GPIO2_Combined_16_31_IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO2);

            if (irq_mask & ((1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT)) |
                            (1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT)) |
                            (1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH)))) {
                xHigherPriorityTaskWoken |= hal::key_input::GPIO2SwitchesIRQHandler(irq_mask);
            }

            if (irq_mask & (1 << static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_ACOK_PIN))) {
                xHigherPriorityTaskWoken |= hal::battery::charger_irq();
            }

            // Clear all IRQs
            GPIO_PortClearInterruptFlags(GPIO2, irq_mask);

            // Switch context if necessary
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }

        void GPIO3_Combined_16_31_IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO3);

            if (irq_mask & (1 << BOARD_EINK_BUSY_GPIO_PIN)) {
                xHigherPriorityTaskWoken |= BSP_EinkBusyPinStateChangeHandler();
            }

            // Clear all IRQs on the GPIO3 port
            GPIO_PortClearInterruptFlags(BOARD_EINK_BUSY_GPIO, irq_mask);

            // Switch context if necessary
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }

        void GPIO5_Combined_0_15_IRQHandler(void)
        {
            uint32_t irq_mask = GPIO_GetPinsInterruptFlags(GPIO5);

            BaseType_t xHigherPriorityTaskWoken = hal::key_input::GPIO5SwitchesIRQHandler(
                1 << static_cast<uint32_t>(BoardDefinitions::BELL_CENTER_SWITCH));
            // Clear all IRQs on the GPIO5 port
            GPIO_PortClearInterruptFlags(GPIO5, irq_mask);

            // Switch context if necessary
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }

        void TMR3_IRQHandler(void)
        {
            QTMR_ClearStatusFlags(TMR3,
                                  kQTMR_Channel_0,
                                  kQTMR_CompareFlag | kQTMR_Compare1Flag | kQTMR_Compare2Flag | kQTMR_OverflowFlag |
                                      kQTMR_EdgeFlag);
            hal::key_input::EncoderIRQHandler();
        }

        /* RTWDOG's timeout asserts IRQ, then 255 bus clocks after interrupt vector fetch
         * forces CPU reset (see RT1050 RM, p.3120). However, this forced reset only
         * affects the CPU, it won't reset any external ICs. To resolve this limitation,
         * WDOG_B pin, which controls board's main DCDC converter, is asserted in RTWDOG
         * IRQ handler. Converter will be momentarily switched off and the whole board will
         * be power reset.
         *
         * The simplest way to do it is to configure the pin as WDOG_B and use WDOG1 built-in
         * assertion.
         *
         * Before proceeding to exception handler, CPU pushes the following registers
         * to the active stack:
         * r0,       offset 0 (in words),
         * r1        offset 1,
         * r2,       offset 2,
         * r3,       offset 3,
         * r12,      offset 4,
         * r14 (LR), offset 5,
         * r15 (PC), offset 6,
         * xPSR,     offset 7.
         * Get the value of last PC state and store in non-volatile SNVS register
         * to have any data that can be used to debug if program died in IRQ or
         * critical section and neither log nor crashdump was created. */
        __attribute__((used, noreturn)) void RTWDOG_Handler(const uint32_t *sp)
        {
            RTWDOG_ClearStatusFlags(RTWDOG, kRTWDOG_InterruptFlag);

            const uint32_t pc = sp[6];
            SNVS->LPGPR[1]    = pc;

            WDOG1->WCR &= ~WDOG_WCR_WDA_MASK;
            while (true) {}; // Execution should never reach this point, but just in case
        }

        /* This has to be done as naked function to be able to easily extract last
         * PC value before RTWDOG handler was called without having to deal with
         * function prologue. */
        __attribute__((naked)) void RTWDOG_IRQHandler(void)
        {
            __asm__ __volatile__("tst lr, #4\n" // Determine which stack was used
                                 "ite eq\n"
                                 "mrseq r0, msp\n"
                                 "mrsne r0, psp\n"
                                 "b RTWDOG_Handler\n" // Jump to real handler
            );
        }

        // Enable PMU brownout interrupt
        void ANATOP_EVENT0_IRQHandler(void)
        {
            const uint32_t status = PMU_GetStatusFlags(PMU);

            /* If the PMU brownout detects too low voltage
             * immediately reset board's main DCDC converter
             * using the WDOG_B pin. This will reset the
             * whole board. */
            if (status & kPMU_1P1BrownoutOnOutput) {
                WDOG1->WCR &= ~WDOG_WCR_WDA_MASK;
            }

            if (status & kPMU_2P5BrownoutOnOutput) {
                WDOG1->WCR &= ~WDOG_WCR_WDA_MASK;
            }

            NVIC_ClearPendingIRQ(ANATOP_EVENT0_IRQn);
        }
    }
} // namespace bsp
