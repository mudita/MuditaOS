// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void vConfigureTimerForRunTimeStats(void);
    uint32_t ulHighFrequencyTimerTicks(void);
    uint32_t ulHighFrequencyTimerMs(void);
    uint32_t ulHighFrequencyTimerTicksToMs(uint32_t ticks);

#ifdef __cplusplus
}
#endif
