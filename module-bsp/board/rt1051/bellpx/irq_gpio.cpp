// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "board/irq_gpio.hpp"
#include <Utils.hpp>

#include "board.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "fsl_common.h"
#include <fsl_qtmr.h>

#include "board/rt1051/bsp/eink/bsp_eink.h"
#include <hal/key_input/KeyInput.hpp>
#include <hal/battery_charger/BatteryChargerIRQ.hpp>
#include "board/BoardDefinitions.hpp"
#include "bsp/light_sensor/light_sensor.hpp"

namespace bsp
{

    void irq_gpio_Init(void)
    {
        DisableIRQ(GPIO1_Combined_0_15_IRQn);
        DisableIRQ(GPIO1_Combined_16_31_IRQn);
        DisableIRQ(GPIO2_Combined_0_15_IRQn);
        DisableIRQ(GPIO2_Combined_16_31_IRQn);
        DisableIRQ(GPIO3_Combined_16_31_IRQn);
        DisableIRQ(GPIO5_Combined_0_15_IRQn);
        DisableIRQ(TMR3_IRQn);

        GPIO_PortDisableInterrupts(GPIO1, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO2, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO3, UINT32_MAX);
        GPIO_PortClearInterruptFlags(GPIO5, UINT32_MAX);

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

        EnableIRQ(GPIO5_Combined_0_15_IRQn);
        NVIC_SetPriority(GPIO5_Combined_0_15_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);

        EnableIRQ(TMR3_IRQn);
        NVIC_SetPriority(TMR3_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
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

            if (irq_mask & (1 << BOARD_BATTERY_CHARGER_INOKB_PIN)) {}

            if (irq_mask & (1 << BOARD_BATTERY_CHARGER_WCINOKB_PIN)) {}

            if (irq_mask & (1 << BOARD_BATTERY_CHARGER_INTB_PIN)) {
                xHigherPriorityTaskWoken |= hal::battery::charger_irq();
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

            if (irq_mask & ((1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_CENTER)) |
                            (1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_RIGHT)) |
                            (1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LEFT)) |
                            (1 << static_cast<uint32_t>(BoardDefinitions::BELL_SWITCHES_LATCH)))) {
                xHigherPriorityTaskWoken |= hal::key_input::generalIRQHandler(irq_mask);
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
            BaseType_t xHigherPriorityTaskWoken = 0;
            uint32_t irq_mask                   = GPIO_GetPinsInterruptFlags(GPIO5);

            if (irq_mask & (1 << static_cast<uint32_t>(BoardDefinitions::BELL_WAKEUP))) {

                xHigherPriorityTaskWoken |= hal::key_input::wakeupIRQHandler();
            }

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
            const uint8_t QTMR3_mask = 99;
            hal::key_input::generalIRQHandler(QTMR3_mask);
        }
    }
} // namespace bsp
