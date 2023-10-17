// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "board/irq_gpio.hpp"

#include "board.h"
#include "FreeRTOS.h"
#include <fsl_common.h>
#include <fsl_rtwdog.h>

#include "board/rt1051/bsp/eink/bsp_eink.h"
#include <hal/key_input/KeyInput.hpp>
#include <hal/battery_charger/BatteryChargerIRQ.hpp>
#include <log/log.hpp>
#include "bsp/cellular/bsp_cellular.hpp"
#include "bsp/headset/headset.hpp"

namespace bsp
{
    void irq_gpio_Init(void)
    {
        DisableIRQ(GPIO1_Combined_0_15_IRQn);
        DisableIRQ(GPIO1_Combined_16_31_IRQn);
        DisableIRQ(GPIO2_Combined_0_15_IRQn);
        DisableIRQ(GPIO2_Combined_16_31_IRQn);
        DisableIRQ(GPIO3_Combined_16_31_IRQn);
        DisableIRQ(RTWDOG_IRQn);

        GPIO_PortDisableInterrupts(GPIO1, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO2, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO3, UINT32_MAX);

        // Clear all IRQs
        GPIO_PortClearInterruptFlags(GPIO1, UINT32_MAX);
        GPIO_PortClearInterruptFlags(GPIO2, UINT32_MAX);
        GPIO_PortClearInterruptFlags(GPIO3, UINT32_MAX);

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

        NVIC_ClearPendingIRQ(RTWDOG_IRQn);
        NVIC_EnableIRQ(RTWDOG_IRQn);
    }

    extern "C"
    {
        void GPIO1_Combined_0_15_IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO1);

            if (irq_mask & (1 << BSP_CELLULAR_STATUS_PIN)) {
                xHigherPriorityTaskWoken |= cellular::status::statusIRQhandler();
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

            if (irq_mask & (1 << BSP_BLUETOOTH_UART_CTS_PIN)) {
                LOG_DEBUG("CTS IRQ!");
            }

            // Clear all IRQs
            GPIO_PortClearInterruptFlags(GPIO1, irq_mask);

            // Switch context if necessary
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }

        void GPIO2_Combined_0_15_IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO2);

            if (irq_mask & (1 << BOARD_KEYBOARD_RF_BUTTON_PIN)) {
                xHigherPriorityTaskWoken |= hal::key_input::rightFunctionalIRQHandler();
            }

            if (irq_mask & (1 << BOARD_BATTERY_CHARGER_INTB_PIN)) {
                xHigherPriorityTaskWoken |= hal::battery::INTBHandlerIRQ();
            }

            if (irq_mask & (1 << BSP_CELLULAR_SIM_TRAY_INSERTED_PIN)) {
                xHigherPriorityTaskWoken |= bsp::cellular::sim::trayIRQHandler();
            }

            // Clear all IRQs
            GPIO_PortClearInterruptFlags(GPIO2, irq_mask);

            // Switch context if necessary
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }

        void GPIO2_Combined_16_31_IRQHandler(void)
        {
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO2);

            if (irq_mask & (1 << BOARD_KEYBOARD_IRQ_GPIO_PIN)) {
                xHigherPriorityTaskWoken |= hal::key_input::generalIRQHandler(irq_mask);
            }

            if (irq_mask & (1 << BOARD_USBC_NINT_PIN)) {
                // TODO:M.P xHigherPriorityTaskWoken |= bsp_usbc_IRQHandler();
            }

            if (irq_mask & (1 << BOARD_JACKDET_IRQ_GPIO_PIN)) {
                xHigherPriorityTaskWoken |= bsp::headset::headset_IRQHandler();
            }

            if (irq_mask & (1 << BSP_CELLULAR_RI_PIN)) {
                bsp::cellular::ringIndicator::riIRQHandler();
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

        /* Before proceeding to exception handler, CPU pushes the following registers
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

            while (true) {}; // Wait for RTWDOG to reset the board
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
    }
} // namespace bsp
