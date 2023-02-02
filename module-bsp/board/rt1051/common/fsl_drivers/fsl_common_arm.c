/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.common_arm"
#endif

// This is part of newer version drivers from NXP

#if defined(SDK_DELAY_USE_DWT) && defined(DWT)
/* Use WDT. */
static void enableCpuCycleCounter(void)
{
    /* Make sure the DWT trace fucntion is enabled. */
    if (CoreDebug_DEMCR_TRCENA_Msk != (CoreDebug_DEMCR_TRCENA_Msk & CoreDebug->DEMCR))
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }

    /* CYCCNT not supported on this device. */
    assert(DWT_CTRL_NOCYCCNT_Msk != (DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk));

    /* Read CYCCNT directly if CYCCENT has already been enabled, otherwise enable CYCCENT first. */
    if (DWT_CTRL_CYCCNTENA_Msk != (DWT_CTRL_CYCCNTENA_Msk & DWT->CTRL))
    {
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

static uint32_t getCpuCycleCount(void)
{
    return DWT->CYCCNT;
}
#else                 /* defined(SDK_DELAY_USE_DWT) && defined(DWT) */
/* Use software loop. */
#if defined(__CC_ARM) /* This macro is arm v5 specific */
/* clang-format off */
__ASM static void DelayLoop(uint32_t count)
{
loop
    SUBS R0, R0, #1
    CMP  R0, #0
    BNE  loop
    BX   LR
}
/* clang-format on */
#elif defined(__ARMCC_VERSION) || defined(__ICCARM__) || defined(__GNUC__)
/* Cortex-M0 has a smaller instruction set, SUBS isn't supported in thumb-16 mode reported from __GNUC__ compiler,
 * use SUB and CMP here for compatibility */
static void DelayLoop(uint32_t count)
{
    __ASM volatile("    MOV    R0, %0" : : "r"(count));
    __ASM volatile(
        "loop:                          \n"
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
        "    SUB    R0, R0, #1          \n"
#else
        "    SUBS   R0, R0, #1          \n"
#endif
        "    CMP    R0, #0              \n"

        "    BNE    loop                \n"
        :
        :
        : "r0");
}
#endif /* defined(__CC_ARM) */
#endif /* defined(SDK_DELAY_USE_DWT) && defined(DWT) */

/*!
 * @brief Delay at least for some time.
 *  Please note that, if not uses DWT, this API will use while loop for delay, different run-time environments have
 *  effect on the delay time. If precise delay is needed, please enable DWT delay. The two parmeters delayTime_us and
 *  coreClock_Hz have limitation. For example, in the platform with 1GHz coreClock_Hz, the delayTime_us only supports
 *  up to 4294967 in current code. If long time delay is needed, please implement a new delay function.
 *
 * @param delayTime_us  Delay time in unit of microsecond.
 * @param coreClock_Hz  Core clock frequency with Hz.
 */
void SDK_DelayAtLeastUs(uint32_t delayTime_us, uint32_t coreClock_Hz)
{
    uint64_t count;

    if (delayTime_us > 0U)
    {
        count = USEC_TO_COUNT(delayTime_us, coreClock_Hz);

        assert(count <= UINT32_MAX);

#if defined(SDK_DELAY_USE_DWT) && defined(DWT) /* Use DWT for better accuracy */

        enableCpuCycleCounter();
        /* Calculate the count ticks. */
        count += getCpuCycleCount();

        if (count > UINT32_MAX)
        {
            count -= UINT32_MAX;
            /* Wait for cyccnt overflow. */
            while (count < getCpuCycleCount())
            {
            }
        }

        /* Wait for cyccnt reach count value. */
        while (count > getCpuCycleCount())
        {
        }
#else
        /* Divide value may be different in various environment to ensure delay is precise.
         * Every loop count includes three instructions, due to Cortex-M7 sometimes executes
         * two instructions in one period, through test here set divide 1.5. Other M cores use
         * divide 4. By the way, divide 1.5 or 4 could let the count lose precision, but it does
         * not matter because other instructions outside while loop is enough to fill the time.
         */
#if (__CORTEX_M == 7)
        count = count / 3U * 2U;
#else
        count = count / 4U;
#endif
        DelayLoop((uint32_t)count);
#endif /* defined(SDK_DELAY_USE_DWT) && defined(DWT) */
    }
}
