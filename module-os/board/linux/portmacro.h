/*
	FreeRTOS.org V5.2.0 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License (version 2) as published
	by the Free Software Foundation and modified by the FreeRTOS exception.

	FreeRTOS.org is distributed in the hope that it will be useful,	but WITHOUT
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


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdint.h>    

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		int
#define portSHORT		short
#define portSTACK_TYPE  unsigned long
#define portBASE_TYPE   long
#define portPOINTER_SIZE_TYPE size_t

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;


#if( configUSE_16_BIT_TICKS == 1 )
    typedef uint16_t TickType_t;
	#define portMAX_DELAY ( portTickType ) 0xffff
#else
    typedef uint32_t TickType_t;
	#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portTICK_RATE_MICROSECONDS		( ( portTickType ) 1000000 / configTICK_RATE_HZ )
#define portINLINE __inline

#if defined( __x86_64__) || defined( _M_X64 )
	#define portBYTE_ALIGNMENT		8
#else
	#define portBYTE_ALIGNMENT		4
#endif

#define portREMOVE_STATIC_QUALIFIER
/*-----------------------------------------------------------*/


/* Scheduler utilities. */
extern void vPortYieldFromISR( void );
extern void vPortYield( void );

#define portYIELD()					vPortYield()

#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired ) vPortYieldFromISR()
/*-----------------------------------------------------------*/


/* Critical section management. */
extern void vPortDisableInterrupts( void );
extern void vPortEnableInterrupts( void );
#define portSET_INTERRUPT_MASK()	( vPortDisableInterrupts() )
#define portCLEAR_INTERRUPT_MASK()	( vPortEnableInterrupts() )

extern portBASE_TYPE xPortSetInterruptMask( void );
extern void vPortClearInterruptMask( portBASE_TYPE xMask );

#define portSET_INTERRUPT_MASK_FROM_ISR()		xPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortClearInterruptMask(x)


extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portDISABLE_INTERRUPTS()	portSET_INTERRUPT_MASK()
#define portENABLE_INTERRUPTS()		portCLEAR_INTERRUPT_MASK()
#define portENTER_CRITICAL()		vPortEnterCritical()
#define portEXIT_CRITICAL()			vPortExitCritical()
/*-----------------------------------------------------------*/


#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
#error We are not supporting configUSE_PORT_OPTIMISED_TASK_SELECTION in the Linux Simulator.
#endif 


/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#define portNOP()

#define portOUTPUT_BYTE( a, b )

extern void vPortForciblyEndThread( void *pxTaskToDelete );
#define traceTASK_DELETE( pxTaskToDelete )		vPortForciblyEndThread( pxTaskToDelete )

extern void vPortAddTaskHandle( void *pxTaskHandle );
#define traceTASK_CREATE( pxNewTCB )			vPortAddTaskHandle( pxNewTCB )

/* Posix Signal definitions that can be changed or read as appropriate. */
#define SIG_SUSPEND					SIGUSR1
#define SIG_RESUME					SIGUSR2

/* Enable the following hash defines to make use of the real-time tick where time progresses at real-time.*/ 
#define SIG_TICK					SIGALRM
#define TIMER_TYPE					ITIMER_REAL 
/* Enable the following hash defines to make use of the process tick where time progresses only when the process is executing.
#define SIG_TICK					SIGVTALRM
#define TIMER_TYPE					ITIMER_VIRTUAL */
/* Enable the following hash defines to make use of the profile tick where time progresses when the process or system calls are executing.
#define SIG_TICK					SIGPROF
#define TIMER_TYPE					ITIMER_PROF */

BaseType_t xPortIsInsideInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

