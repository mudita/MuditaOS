// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "pit.hpp"
#include <board.h>
#include <board/clock_config.h>
#include <fsl_pit.h>
#include <board/pin_mux.h>
#include <stdint.h>

struct PIT_t
{
    uint32_t usec        = 1000000;
    xQueueHandle qhandle = nullptr;
    std::function<void()> irq_cb;
};

static PIT_t LPIT;

static void pit_init(xQueueHandle qhandle);
static void pit_start(uint32_t usec, std::function<void(void)> cb);
static void pit_stop();

namespace bsp
{
    namespace pit
    {
        void init(xQueueHandle qhandle)
        {
            pit_init(qhandle);
        }

        void start(uint32_t usec, std::function<void(void)> cb)
        {
            pit_start(usec, cb);
        }

        void stop()
        {
            pit_stop();
        }

    }; // namespace pit
};     // namespace bsp

/// bsp

static void pit_init(xQueueHandle qhandle)
{
    LPIT.qhandle = qhandle;
    pit_config_t pitConfig;
    PIT_GetDefaultConfig(&pitConfig);
    PIT_Init(PIT, &pitConfig);
}

static void pit_start(uint32_t usec, std::function<void(void)> cb)
{
    LPIT.usec = usec;
    DisableIRQ(PIT_IRQn);
    PIT_DisableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);

    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(usec, CLOCK_GetFreq(kCLOCK_OscClk)));
    LPIT.irq_cb = cb;

    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    EnableIRQ(PIT_IRQn);

    PIT_StartTimer(PIT, kPIT_Chnl_0);
}

static void pit_stop()
{
    PIT_StopTimer(PIT, kPIT_Chnl_0);
}

extern "C"
{
    void PIT_IRQHandler(void)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (LPIT.qhandle) {
            auto val = bsp::pit::Event::Overflow;
            xQueueSendFromISR(LPIT.qhandle, &val, &xHigherPriorityTaskWoken);
        }
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
        __DSB();
        /// stop timer - we are interested in one time run
        pit_stop();
        if (LPIT.irq_cb)
            LPIT.irq_cb();
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
};
