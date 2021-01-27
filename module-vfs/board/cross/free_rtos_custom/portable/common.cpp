// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * common.c
 *
 *  Created on: 2 Feb 2018
 *      Author: mati
 */
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <time/time_conversion.hpp>
#if 0
struct tm *gmtime_r( const time_t *pxTime, struct tm *tmStruct )
{
	// Dummy time functions to keep the file system happy in the absence of
	//target support.
	memcpy( tmStruct, gmtime( pxTime ), sizeof( * tmStruct ) );
	return tmStruct;
}
#endif
/*-----------------------------------------------------------*/
extern "C"
{
    time_t FreeRTOS_time(time_t *pxTime)
    {
        return utils::time::Time().getTime();
    }
}
