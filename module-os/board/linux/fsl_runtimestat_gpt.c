// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t prof_tick = 0;

static void TickSignalHandler( int sig )
{
    ++prof_tick;
}

void vConfigureTimerForRunTimeStats(void)
{
    struct sigaction tick;
    sigemptyset(&tick.sa_mask);
    tick.sa_flags = 0;
    tick.sa_handler = TickSignalHandler;

    if (sigaction( SIGPROF, &tick, NULL )!=0)
    //if (sigaction( SIGVTALRM, &tick, NULL )!=0)
    {
        printf("cant profile\n");
        exit(1);
    }

    struct itimerval itimer;
    if( getitimer(ITIMER_PROF , &itimer) != 0 ){
        printf("cant get profile\n");
        exit(1);
    }
    /// timer set to 1kHz, not 1MHz like timer in the other gpt
    /// freq is the same as freq of rtos
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 1000;
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 1000;

    /* Set-up the timer interrupt. */
    if (setitimer( ITIMER_PROF, &itimer, NULL) != 0 )
    {
        printf("cant set profile\n");
        exit(1);
    }
}

uint32_t ulHighFrequencyTimerTicks(void)
{
    return prof_tick;
}
