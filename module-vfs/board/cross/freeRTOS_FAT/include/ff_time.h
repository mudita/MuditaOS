/*
 * FreeRTOS+FAT Labs Build 160919 (C) 2016 Real Time Engineers ltd.
 * Authors include James Walmsley, Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   FREERTOS+FAT IS STILL IN THE LAB:                                     ***
 ***                                                                         ***
 ***   This product is functional and is already being used in commercial    ***
 ***   products.  Be aware however that we are still refining its design,    ***
 ***   the source code does not yet fully conform to the strict coding and   ***
 ***   style standards mandated by Real Time Engineers ltd., and the         ***
 ***   documentation and testing is not necessarily complete.                ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact  Active early adopters may, at   ***
 ***   the sole discretion of Real Time Engineers Ltd., be offered versions  ***
 ***   under a license other than that described below.                      ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
 *
 * FreeRTOS+FAT can be used under two different free open source licenses.  The
 * license that applies is dependent on the processor on which FreeRTOS+FAT is
 * executed, as follows:
 *
 * If FreeRTOS+FAT is executed on one of the processors listed under the Special
 * License Arrangements heading of the FreeRTOS+FAT license information web
 * page, then it can be used under the terms of the FreeRTOS Open Source
 * License.  If FreeRTOS+FAT is used on any other processor, then it can be used
 * under the terms of the GNU General Public License V2.  Links to the relevant
 * licenses follow:
 *
 * The FreeRTOS+FAT License Information Page: http://www.FreeRTOS.org/fat_license
 * The FreeRTOS Open Source License: http://www.FreeRTOS.org/license
 * The GNU General Public License Version 2: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * FreeRTOS+FAT is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+FAT unless you agree that you use the software 'as is'.
 * FreeRTOS+FAT is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/plus
 * http://www.FreeRTOS.org/labs
 *
 */

/**
 *	@file		ff_time.h
 *	@ingroup	TIME
 *
 *	Provides a means for receiving the time on any platform.
 **/

#ifndef _FF_TIME_H_
#define _FF_TIME_H_

#include <time.h>

#include "FreeRTOSFATConfig.h"

/* _HT_
The following declarations and functions may be moved to a common directory?
 */
typedef struct xTIME_STRUCT
{
	int tm_sec;   /* Seconds */
	int tm_min;   /* Minutes */
	int tm_hour;  /* Hour (0--23) */
	int tm_mday;  /* Day of month (1--31) */
	int tm_mon;   /* Month (0--11) */
	int tm_year;  /* Year (calendar year minus 1900) */
	int tm_wday;  /* Weekday (0--6; Sunday = 0) */
	int tm_yday;  /* Day of year (0--365) */
	int tm_isdst; /* 0 if daylight savings time is not in effect) */
} FF_TimeStruct_t;

/* Equivalent of time() : returns the number of seconds after 1-1-1970. */
time_t FreeRTOS_time( time_t *pxTime );

/* Equivalent of mktime() : calculates the number of seconds after 1-1-1970. */
time_t FreeRTOS_mktime( const FF_TimeStruct_t *pxTimeBuf );

/* Equivalent of gmtime_r() : Fills a 'struct tm'. */
FF_TimeStruct_t *FreeRTOS_gmtime_r( const time_t *pxTime, FF_TimeStruct_t *pxTimeBuf );

/**
 *	@public
 *	@brief	A TIME and DATE object for FreeRTOS+FAT. A FreeRTOS+FAT time driver must populate these values.
 *
 **/
typedef struct
{
	uint16_t Year;		/* Year	(e.g. 2009). */
	uint16_t Month;		/* Month	(e.g. 1 = Jan, 12 = Dec). */
	uint16_t Day;		/* Day	(1 - 31). */
	uint16_t Hour;		/* Hour	(0 - 23). */
	uint16_t Minute;	/* Min	(0 - 59). */
	uint16_t Second;	/* Second	(0 - 59). */
} FF_SystemTime_t;

/*---------- PROTOTYPES */

int32_t	FF_GetSystemTime(FF_SystemTime_t *pxTime);

#endif

