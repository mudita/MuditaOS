// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <fsl_runtimestat_gpt.h>
#include <fsl_gpt.h>

static const uint32_t millisecondsInSecond = 1000U;
static const uint32_t runTimeStatsClockHz  = 10000U;

static inline GPT_Type *vPortGetGptBase(void)
{
    return GPT1;
}

void vConfigureTimerForRunTimeStats(void)
{
    const gpt_config_t GPT1_config = {.clockSource = kGPT_ClockSource_Periph,
                                      // freq = (f_Osc / divider) -> (24 MHz / 2400) = 10 kHz
                                      .divider         = 2400UL,
                                      .enableFreeRun   = false,
                                      .enableRunInWait = true,
                                      .enableRunInStop = true,
                                      .enableRunInDoze = false,
                                      .enableRunInDbg  = false,
                                      .enableMode      = true};

    GPT_Type *pxGptBase = vPortGetGptBase();

    /* GPT device and channels initialization */
    GPT_Init(pxGptBase, &GPT1_config);
    GPT_SetOscClockDivider(pxGptBase, 1);
    /* Enable GPT interrupt sources */
    GPT_EnableInterrupts(pxGptBase, 0);
    /* Start the GPT timer */
    GPT_StartTimer(pxGptBase);
}

uint32_t ulHighFrequencyTimerTicksToMs(uint32_t ticks)
{
    return (ticks / (runTimeStatsClockHz / millisecondsInSecond));
}

uint32_t ulHighFrequencyTimerTicks(void)
{
    return GPT_GetCurrentTimerCount(vPortGetGptBase());
}

uint32_t ulHighFrequencyTimerMs(void)
{
    return ulHighFrequencyTimerTicksToMs(ulHighFrequencyTimerTicks());
}
