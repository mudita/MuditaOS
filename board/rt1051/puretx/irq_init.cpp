// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <board/irq_init.hpp>
#include <fsl_common.h>
#include <fsl_gpio.h>

namespace board
{
    void irq_gpio_Init(void)
    {
        DisableIRQ(GPIO1_Combined_0_15_IRQn);
        DisableIRQ(GPIO1_Combined_16_31_IRQn);
        DisableIRQ(GPIO2_Combined_0_15_IRQn);
        DisableIRQ(GPIO2_Combined_16_31_IRQn);
        DisableIRQ(GPIO3_Combined_16_31_IRQn);

        GPIO_PortDisableInterrupts(GPIO1, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO2, UINT32_MAX);
        GPIO_PortDisableInterrupts(GPIO3, UINT32_MAX);

        // Clear all IRQs
        GPIO_PortClearInterruptFlags(GPIO1, UINT32_MAX);
        GPIO_PortClearInterruptFlags(GPIO2, UINT32_MAX);
        GPIO_PortClearInterruptFlags(GPIO3, UINT32_MAX);

        EnableIRQ(GPIO1_Combined_0_15_IRQn);
        NVIC_SetPriority(GPIO1_Combined_0_15_IRQn, lowestIRQprio);

        EnableIRQ(GPIO1_Combined_16_31_IRQn);
        NVIC_SetPriority(GPIO1_Combined_16_31_IRQn, lowestIRQprio);

        EnableIRQ(GPIO2_Combined_0_15_IRQn);
        NVIC_SetPriority(GPIO2_Combined_0_15_IRQn, lowestIRQprio);

        EnableIRQ(GPIO2_Combined_16_31_IRQn);
        NVIC_SetPriority(GPIO2_Combined_16_31_IRQn, lowestIRQprio);

        EnableIRQ(GPIO3_Combined_16_31_IRQn);
        NVIC_SetPriority(GPIO3_Combined_16_31_IRQn, lowestIRQprio);
    }

} // namespace board
