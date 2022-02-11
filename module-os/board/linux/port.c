/*
 *  Copyright (C) 2017 Michael Becker (michael.f.becker@gmail.com
 *  Modified from code from William Davy (william.davy@wittenstein.co.uk)
 *
 *  Modifications (not exhaustive):
 *  ---------------------------------------------
 *  + Updated to work with FreeRTOS v8.2.3
 *
 *  + Updated code that works with pthreads to 
 *    treat pthread_t data as opaque data structures,
 *    as specified in the POSIX standard. Likewise, 
 *    cannot initialize them to NULL, not portable.
 *
 *  + Since FreeRTOS is not multicore, we are forcing 
 *    all of the pthreads that simulate FreeRTOS tasks 
 *    to execute on a single core.
 *
 */
/*
    Copyright (C) 2009 William Davy - william.davy@wittenstein.co.uk
    Contributed to FreeRTOS.org V5.3.0.

    This file is part of the FreeRTOS.org distribution.

    FreeRTOS.org is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License (version 2) as published
    by the Free Software Foundation and modified by the FreeRTOS exception.

    FreeRTOS.org is distributed in the hope that it will be useful,    but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details.

    You should have received a copy of the GNU General Public License along
    with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
    Temple Place, Suite 330, Boston, MA  02111-1307  USA.

    A special exception to the GPL is included to allow you to distribute a
    combined work that includes FreeRTOS.org without being obliged to provide
    the source code for any proprietary components.  See the licensing section
    of http://www.FreeRTOS.org for full details.


    ***************************************************************************
    *                                                                         *
    * Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
    *                                                                         *
    * This is a concise, step by step, 'hands on' guide that describes both   *
    * general multitasking concepts and FreeRTOS specifics. It presents and   *
    * explains numerous examples that are written using the FreeRTOS API.     *
    * Full source code for all the examples is provided in an accompanying    *
    * .zip file.                                                              *
    *                                                                         *
    ***************************************************************************

    1 tab == 4 spaces!

    Please ensure to read the configuration and relevant port sections of the
    online documentation.

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the Posix port.
 *----------------------------------------------------------*/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "macros.h"

/* Each task maintains its own interrupt status in the critical nesting variable. */
typedef struct ThreadState_t_
{
    pthread_t       Thread;
    int             Valid;    /* Treated as a boolean */
    xTaskHandle     hTask;
    portBASE_TYPE   uxCriticalNesting;
    pdTASK_CODE     pxCode;
    void            *pvParams;
    int             index;

} ThreadState_t;


#define MAX_NUMBER_OF_TASKS ( _POSIX_THREAD_THREADS_MAX )
static ThreadState_t pxThreads[MAX_NUMBER_OF_TASKS];

static pthread_once_t hSigSetupThread = PTHREAD_ONCE_INIT;

static pthread_mutex_t xSuspendResumeThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t xSingleThreadMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t hMainThread;
static pthread_t hEndSchedulerCallerThread;
int hEndSchedulerCallerThreadIndex;

static volatile portBASE_TYPE xSentinel = 0;
static volatile portBASE_TYPE xSchedulerEnd = pdFALSE;
static volatile portBASE_TYPE xInterruptsEnabled = pdTRUE;
static volatile portBASE_TYPE xServicingTick = pdFALSE;
static volatile portBASE_TYPE xPendYield = pdFALSE;
static volatile portLONG lIndexOfLastAddedTaskStack[_POSIX_THREAD_THREADS_MAX];
static volatile portLONG lLastAddedTaskStackIdx = 0;
static volatile portBASE_TYPE uxCriticalNesting;


/**
 *  pthread cleanup routine, always executed on pthread exit.
 */
static void DeleteThreadCleanupRoutine( void *Parameter )
{
    ThreadState_t *State = (ThreadState_t *)Parameter;

#if LINUX_PORT_DEBUG
    printf("[%d] DeleteThreadCleanupRoutine for index\n", State->index);
#endif

    State->Valid = 0;
    State->hTask = (xTaskHandle)NULL;
    if ( State->uxCriticalNesting > 0 )
    {
        State->uxCriticalNesting = 0;
        uxCriticalNesting = 0;
        vPortEnableInterrupts();
    }
}


/**
 *  Sends SIG_SUSPEND to target thread.
 */
static void SuspendThread( pthread_t Thread )
{
    pthread_mutex_lock( &xSuspendResumeThreadMutex );

    /* Set-up for the Suspend Signal handler? */
    xSentinel = 0;
    pthread_mutex_unlock( &xSuspendResumeThreadMutex );

    pthread_kill( Thread, SIG_SUSPEND );
    while ( ( xSentinel == 0 ) && ( pdTRUE != xServicingTick ) )
    {
        sched_yield();
    }
}

/**
 *  Signal handler for SIG_SUSPEND
 */
static void SuspendSignalHandler(int sig)
{
    sigset_t xSignals;
    int rc;

    /* Only interested in the resume signal. */
    sigemptyset( &xSignals );
    sigaddset( &xSignals, SIG_RESUME );
    xSentinel = 1;

    /* Unlock the Single thread mutex to allow the resumed task to continue. */
    rc = pthread_mutex_unlock(&xSingleThreadMutex);
    assert(rc == 0);

    /* Wait on the resume signal. */
    rc = sigwait(&xSignals, &sig);
    assert(rc == 0);
    
    /* Will resume here when the SIG_RESUME signal is received. */
    /* Need to set the interrupts based on the task's critical nesting. */
    if ( uxCriticalNesting == 0 )
    {
        vPortEnableInterrupts();
    }
    else
    {
        vPortDisableInterrupts();
    }
}


/**
 *  Signal handler for SIG_RESUME.
 */
static void ResumeSignalHandler(int sig)
{
    (void)sig;

    /* Yield the Scheduler to ensure that the yielding thread completes. */
    if (pthread_mutex_lock(&xSingleThreadMutex) == 0) {
        pthread_mutex_unlock(&xSingleThreadMutex);
    }
}


/**
 *  Sends SIG_RESUME to target thread.
 */
static void ResumeThread( pthread_t Thread )
{
    int rc;

    rc = pthread_mutex_lock(&xSuspendResumeThreadMutex);
    assert(rc == 0);
    
    if ( !pthread_equal(pthread_self(), Thread))
    {
        pthread_kill( Thread, SIG_RESUME );
    }

    pthread_mutex_unlock( &xSuspendResumeThreadMutex );
}


/**
 *  Utility function to lookup a pthread_t based on 
 *  a FreeRTOS Task Handle.
 */
static int LookupThread(xTaskHandle hTask, pthread_t *Thread)
{
    int i;
    
    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if (pxThreads[i].hTask == hTask)
        {
            *Thread = pxThreads[i].Thread;
            return 1;
        }
    }

    return 0;
}


static void prvSetTaskCriticalNesting( pthread_t Thread, portBASE_TYPE uxNesting )
{
    int i;

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if ( pthread_equal(pxThreads[i].Thread, Thread) )
        {
            pxThreads[i].uxCriticalNesting = uxNesting;
            return;
        }
    }

    assert(!"Failed finding pthread for task mapping!");
}


static portBASE_TYPE prvGetTaskCriticalNesting( pthread_t Thread )
{
    unsigned portBASE_TYPE uxNesting = 0;
    int i;

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if ( pthread_equal(pxThreads[i].Thread, Thread) )
        {
            uxNesting = pxThreads[i].uxCriticalNesting;
            return uxNesting;
        }
    }
    
    assert(!"Failed finding pthread for task mapping!");
    return 0;
}


/**
 *  Signal handler for SIG_TICK.
 */
static void TickSignalHandler( int sig )
{
    pthread_t ThreadToSuspend = 0;
    pthread_t ThreadToResume = 0;
    int success;

    (void)sig;

    if ((pdTRUE == xInterruptsEnabled) && (pdTRUE != xServicingTick))
    {
        if ( 0 == pthread_mutex_trylock( &xSingleThreadMutex ) )
        {
            xServicingTick = pdTRUE;

            /*
             *  Required call for newer FreeRTOS kernels
             */
            xTaskIncrementTick();

            success = LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToSuspend);
            assert(success);

            /* Select Next Task. */
#if ( configUSE_PREEMPTION == 1 )
            vTaskSwitchContext();
#endif
            success = LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToResume);
            if (success == 0) {
                LOG_FATAL("Emulator only bug." \
                          "Ignore and restart emulator or refer to commit linked with this log and fix it better :) ");
            }
            assert(success);

            /* The only thread that can process this tick is the running thread. */
            if ( !pthread_equal(ThreadToSuspend, ThreadToResume) )
            {
                /* Remember and switch the critical nesting. */
                prvSetTaskCriticalNesting( ThreadToSuspend, uxCriticalNesting );
                uxCriticalNesting = prvGetTaskCriticalNesting( ThreadToResume );
                /* Resume next task. */
                ResumeThread( ThreadToResume );
                /* Suspend the current task. */
                SuspendThread( ThreadToSuspend );
            }
            else
            {
                /* Release the lock as we are Resuming. */
                pthread_mutex_unlock( &xSingleThreadMutex );
            }
            xServicingTick = pdFALSE;
        }
        else
        {
            xPendYield = pdTRUE;
        }
    }
    else
    {
        xPendYield = pdTRUE;
    }
}


/**
 *  Only run once!
 */
static void prvSetupSignalsAndSchedulerPolicy( void )
{
    /* The following code would allow for configuring the scheduling of this task as a Real-time task.
     * The process would then need to be run with higher privileges for it to take affect.
    int iPolicy;
    int iResult;
    int iSchedulerPriority;
    iResult = pthread_getschedparam( pthread_self(), &iPolicy, &iSchedulerPriority );
    iResult = pthread_attr_setschedpolicy( &xThreadAttributes, SCHED_FIFO );
    iPolicy = SCHED_FIFO;
    iResult = pthread_setschedparam( pthread_self(), iPolicy, &iSchedulerPriority );        */

    int i;
    struct sigaction sigsuspendself, sigresume, sigtick;

    memset(pxThreads, 0, sizeof(pxThreads));

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++ )
    {
        pxThreads[i].index = i;
    }
    

    sigsuspendself.sa_flags = 0;
    sigsuspendself.sa_handler = SuspendSignalHandler;
    sigfillset( &sigsuspendself.sa_mask );

    sigresume.sa_flags = 0;
    sigresume.sa_handler = ResumeSignalHandler;
    sigfillset( &sigresume.sa_mask );

    sigtick.sa_flags = 0;
    sigtick.sa_handler = TickSignalHandler;
    sigfillset( &sigtick.sa_mask );

    if ( 0 != sigaction( SIG_SUSPEND, &sigsuspendself, NULL ) )
    {
        assert( !"Problem installing SIG_SUSPEND_SELF\n" );
    }
    if ( 0 != sigaction( SIG_RESUME, &sigresume, NULL ) )
    {
        assert( !"Problem installing SIG_RESUME\n" );
    }
    if ( 0 != sigaction( SIG_TICK, &sigtick, NULL ) )
    {
        assert( !"Problem installing SIG_TICK\n" );
    }

#if LINUX_PORT_DEBUG
    printf( "Running as PID: %d\n", getpid() );
#endif

    /*
     *  Also save the first thread as the main thread.
     */
    hMainThread = pthread_self();
}


/**
 *  Actual pthreads code, wrapper around FreeRTOS task.
 */
static void *ThreadStartWrapper( void * pvParams )
{
    ThreadState_t *State = (ThreadState_t *)pvParams;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    
    pthread_cleanup_push( DeleteThreadCleanupRoutine, State );

    pthread_mutex_lock(&xSingleThreadMutex); 
    SuspendThread( pthread_self() );

#if LINUX_PORT_DEBUG
    printf("[%d] Starting thread\n", State->index);
#endif

    State->pxCode( State->pvParams );

#if LINUX_PORT_DEBUG
    printf("[%d] Ending thread - SHOULD NEVER SEE THIS\n", State->index);
#endif

    /* make sure we execute DeleteThreadCleanupRoutine */
    pthread_cleanup_pop( 1 );

    return (void *)NULL;
}


/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
    int rc;
    int i;
    pthread_attr_t xThreadAttributes;
    cpu_set_t cpuset;
 
    pthread_once( &hSigSetupThread, prvSetupSignalsAndSchedulerPolicy );

    /* No need to join the threads. */
    pthread_attr_init( &xThreadAttributes );
    pthread_attr_setdetachstate( &xThreadAttributes, PTHREAD_CREATE_DETACHED );

    vPortEnterCritical();

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++ )
    {
        if (pxThreads[i].Valid == 0)
        {
            lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx] = i;
            break;
        }
    }
    /* No more free threads, please increase the maximum. */
    assert(i < MAX_NUMBER_OF_TASKS);


    /* Add the task parameters. */
    pxThreads[lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx]].pxCode = pxCode;
    pxThreads[lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx]].pvParams = pvParameters;

    /* Create the new pThread. */
    rc = pthread_mutex_lock(&xSingleThreadMutex);
    assert(rc == 0);

    xSentinel = 0;

    rc = pthread_create(&pxThreads[lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx]].Thread,
                        &xThreadAttributes, 
                        ThreadStartWrapper, 
                        (void *)&pxThreads[lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx]]
                        );
    assert(rc == 0);

    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);

    rc = pthread_setaffinity_np(pxThreads[lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx]].Thread,
                                sizeof(cpu_set_t), 
                                &cpuset);
	configASSERT( rc == 0 );
    pxThreads[lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx]].Valid = 1;

    lLastAddedTaskStackIdx++;

    /* Wait until the task suspends. */
    pthread_mutex_unlock( &xSingleThreadMutex );

    while ( xSentinel == 0 );
    vPortExitCritical();

    return pxTopOfStack;
}


/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
static void prvSetupTimerInterrupt( void )
{
    int rc;
    struct itimerval itimer, oitimer;
    suseconds_t MicroSeconds = (suseconds_t)(portTICK_RATE_MICROSECONDS % 1000000);
    time_t Seconds = portTICK_RATE_MICROSECONDS / 1000000;

    /* Initialise the structure with the current timer information. */
    rc = getitimer( TIMER_TYPE, &itimer);
    assert(rc == 0);

    /* Set the interval between timer events. */
    itimer.it_interval.tv_sec = Seconds;
    itimer.it_interval.tv_usec = MicroSeconds;

    /* Set the current count-down. */
    itimer.it_value.tv_sec = Seconds;
    itimer.it_value.tv_usec = MicroSeconds;

#if LINUX_PORT_DEBUG
    printf("Timer Setup:\n");
    printf("  Interval: %ld seconds, %ld useconds\n", 
            itimer.it_interval.tv_sec, itimer.it_interval.tv_usec);
    printf("  Current: %ld seconds, %ld useconds\n", 
            itimer.it_value.tv_sec, itimer.it_value.tv_usec);
#endif

    /* Set-up the timer interrupt. */
    rc = setitimer( TIMER_TYPE, &itimer, &oitimer );
    assert(rc == 0);
    
}


/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
    int iSignal;
    sigset_t xSignals;
    sigset_t xSignalToBlock;
    sigset_t xSignalsBlocked;
    pthread_t FirstThread = 0;
    int success;

#if LINUX_PORT_DEBUG
    printf("\n***** LINUX PORT CONFIGURED FOR DEBUG *****\n");
#endif    


    /* Establish the signals to block before they are needed. */
    sigfillset( &xSignalToBlock );

    /* Block until the end */
    pthread_sigmask( SIG_SETMASK, &xSignalToBlock, &xSignalsBlocked );

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    prvSetupTimerInterrupt();

    /* Start the first task. Will not return unless all threads are killed. */
    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    vPortEnableInterrupts();

    success = LookupThread( xTaskGetCurrentTaskHandle(), &FirstThread);
    assert(success);
    

    /* Start the first task. */
    ResumeThread(FirstThread);


    /* This is the end signal we are looking for. */
    sigemptyset( &xSignals );
    sigaddset( &xSignals, SIG_RESUME );

    while ( pdTRUE != xSchedulerEnd )
    {
        if ( 0 != sigwait( &xSignals, &iSignal ) )
        {
            printf( "Main thread spurious signal: %d\n", iSignal );
        }
    }

#if LINUX_PORT_DEBUG
    printf("Exiting scheduler.\n");
    printf("[%d] Canceling xTaskEndScheduler caller thread.\n", hEndSchedulerCallerThreadIndex);
#endif

    pthread_cancel(hEndSchedulerCallerThread);
    sleep(1);

    /* Cleanup the mutexes */
#if LINUX_PORT_DEBUG
    printf( "Freeing OS mutexes.\n" );
#endif

    pthread_mutex_destroy( &xSuspendResumeThreadMutex );
    pthread_mutex_destroy( &xSingleThreadMutex );

    sleep(1);

    memset(&xSuspendResumeThreadMutex,0,sizeof(xSuspendResumeThreadMutex));
    memset(&xSingleThreadMutex,0,sizeof(xSingleThreadMutex));

    hSigSetupThread = PTHREAD_ONCE_INIT;

    hMainThread =0;
    hEndSchedulerCallerThread=0;
    hEndSchedulerCallerThreadIndex=0;

    xSentinel = 0;
    xSchedulerEnd = pdFALSE;
    xInterruptsEnabled = pdTRUE;
    xServicingTick = pdFALSE;
    xPendYield = pdFALSE;
    lLastAddedTaskStackIdx = 0;
    uxCriticalNesting = 0;

    signal(SIG_SUSPEND, SIG_DFL);
    signal(SIG_RESUME, SIG_DFL);
    signal(SIG_TICK, SIG_DFL);

    /* Should not get here! */
    return 0;
}


void vPortEndScheduler( void )
{
    int i;
    int rc;
    struct sigaction sigtickdeinit;

    xInterruptsEnabled = pdFALSE;

    /* Signal the scheduler to exit its loop. */
    xSchedulerEnd = pdTRUE;

    /* Ignore next or pending SIG_TICK, it mustn't execute anymore. */
    sigtickdeinit.sa_flags = 0;
    sigtickdeinit.sa_handler = SIG_IGN;
    sigfillset(&sigtickdeinit.sa_mask);

    rc = sigaction(SIG_TICK, &sigtickdeinit, NULL);
    assert(rc == 0);

    rc = sigaction(SIG_RESUME, &sigtickdeinit, NULL);
    assert(rc == 0);

    rc = sigaction(SIG_SUSPEND, &sigtickdeinit, NULL);
    assert(rc == 0);
    

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if ( pxThreads[i].Valid )
        {
            //pxThreads[i].Valid = 0;
                
            /* Don't kill yourself */
            if (pthread_equal(pxThreads[i].Thread, pthread_self())) 
            {
#if LINUX_PORT_DEBUG
                printf("[%d] Delaying canceling pthread\n", i);
#endif                
                hEndSchedulerCallerThread = pxThreads[i].Thread;
                hEndSchedulerCallerThreadIndex = pxThreads[i].index;
                continue;
            }
            else
            {
                /* Kill all of the threads, they are in the detached state. */
#if LINUX_PORT_DEBUG
                printf("[%d] canceling pthread\n", i);
#endif                
                pthread_cancel(pxThreads[i].Thread );
                sleep(1);
            }
        }
    }

    pthread_kill( hMainThread, SIG_RESUME );
}


void vPortYieldFromISR( void )
{
    /* Calling Yield from a Interrupt/Signal handler often doesn't work because the
     * xSingleThreadMutex is already owned by an original call to Yield. Therefore,
     * simply indicate that a yield is required soon.
     */
    xPendYield = pdTRUE;
}


void vPortEnterCritical( void )
{
    vPortDisableInterrupts();
    uxCriticalNesting++;
}


void vPortExitCritical( void )
{
    /* Check for unmatched exits. */
    if ( uxCriticalNesting > 0 )
    {
        uxCriticalNesting--;
    }

    /* If we have reached 0 then re-enable the interrupts. */
    if( uxCriticalNesting == 0 )
    {
        /* Have we missed ticks? This is the equivalent of pending an interrupt. */
        if ( pdTRUE == xPendYield )
        {
            xPendYield = pdFALSE;
            vPortYield();
        }
        vPortEnableInterrupts();
    }
}


void vPortYield( void )
{
    int rc;
    pthread_t ThreadToSuspend = 0;
    pthread_t ThreadToResume = 0;
    int success;

    rc = pthread_mutex_lock(&xSingleThreadMutex);
    assert(rc == 0);
   

    success = LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToSuspend);
    assert(success);

    vTaskSwitchContext();

    success = LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToResume);
    assert(success);
    

    if ( !pthread_equal(ThreadToSuspend, ThreadToResume) )
    {
        /* Remember and switch the critical nesting. */
        prvSetTaskCriticalNesting( ThreadToSuspend, uxCriticalNesting );
        uxCriticalNesting = prvGetTaskCriticalNesting( ThreadToResume );
        /* Switch tasks. */
        ResumeThread( ThreadToResume );
        SuspendThread( ThreadToSuspend );
    }
    else
    {
        /* Yielding to self */
        pthread_mutex_unlock( &xSingleThreadMutex );
    }
}


void vPortDisableInterrupts( void )
{
    xInterruptsEnabled = pdFALSE;
}


void vPortEnableInterrupts( void )
{
    xInterruptsEnabled = pdTRUE;
}


portBASE_TYPE xPortSetInterruptMask( void )
{
    portBASE_TYPE xReturn = xInterruptsEnabled;
    xInterruptsEnabled = pdFALSE;
    return xReturn;
}


void vPortClearInterruptMask( portBASE_TYPE xMask )
{
    xInterruptsEnabled = xMask;
}


/**
 *  Public API used in tasks.c
 */
void vPortForciblyEndThread( void *pxTaskToDelete )
{
    xTaskHandle hTaskToDelete = ( xTaskHandle )pxTaskToDelete;
    pthread_t ThreadToDelete = 0;
    pthread_t ThreadToResume = 0;
    int success;

    pthread_mutex_lock(&xSingleThreadMutex);

    success = LookupThread(hTaskToDelete, &ThreadToDelete);
    (void)success;

    LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToResume);

    if ( pthread_equal(ThreadToResume, ThreadToDelete) )
    {
        /* This is a suicidal thread, need to select a different task to run. */
        vTaskSwitchContext();
        LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToResume);
    }

    if ( !pthread_equal(pthread_self(), ThreadToDelete) )
    {
        /* Cancelling a thread that is not me. */
 
        /* Send a signal to wake the task so that it definitely cancels. */
        pthread_testcancel();
        pthread_cancel( ThreadToDelete );
 
        /* Pthread Clean-up function will note the cancellation. */
        pthread_mutex_unlock( &xSingleThreadMutex );
    }
    else
    {
        /* Resume the other thread. */
        ResumeThread( ThreadToResume );
        /* Pthread Clean-up function will note the cancellation. */
        /* Release the execution. */
        uxCriticalNesting = 0;
        vPortEnableInterrupts();
        pthread_mutex_unlock( &xSingleThreadMutex );
        /* Commit suicide */
        pthread_exit( (void *)1 );
    }
}


/**
 *  Used for traceTASK_CREATE in FreeRTOS.
 */
void vPortAddTaskHandle( void *pxTaskHandle )
{
    int i;
    vPortEnterCritical();
    lLastAddedTaskStackIdx--;
    pxThreads[lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx]].hTask = (xTaskHandle)pxTaskHandle;

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if ( pthread_equal(pxThreads[i].Thread, pxThreads[lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx]].Thread))
        {
            if ( pxThreads[i].hTask != pxThreads[ lIndexOfLastAddedTaskStack[lLastAddedTaskStackIdx] ].hTask )
            {
                pxThreads[i].Valid = 0;
                pxThreads[i].hTask = NULL;
                pxThreads[i].uxCriticalNesting = 0;
            }
        }
    }
    vPortExitCritical();
}


void vPortFindTicksPerSecond( void )
{
    /* Needs to be reasonably high for accuracy. */
    unsigned long ulTicksPerSecond = sysconf(_SC_CLK_TCK);
    printf( "Timer Resolution for Run TimeStats is %ld ticks per second.\n", ulTicksPerSecond );
}


unsigned long ulPortGetTimerValue( void )
{
    struct tms xTimes;

    (void)times( &xTimes );
    /* Return the application code times.
     * The timer only increases when the application code is actually running
     * which means that the total execution times should add up to 100%.
     */
    return ( unsigned long ) xTimes.tms_utime;
}


BaseType_t xPortIsInsideInterrupt(void)
{
    return pdFALSE;
}

uint32_t CLOCK_GetFreq(int clock) {
    (void)clock;
    return 0;
}
