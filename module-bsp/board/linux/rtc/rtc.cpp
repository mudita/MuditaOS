/*
 * rtc.cpp
 *
 *  Created on: Jun 26, 2019
 *      Author: kuba
 */

#include "rtc/rtc.hpp"
#include <time.h>
#include "FreeRTOS.h"

static time_t timestampOffset;
RtcBspError_e BSP_RtcInit()
{

	timestampOffset = 0;
    return RtcBspOK;
}

RtcBspError_e BSP_RtcSetDateTimeFromTimestamp(time_t timestamp)
{

	time_t current = time(NULL);

	timestampOffset = timestamp - current;
    return RtcBspOK;
}

RtcBspError_e BSP_RtcSetDateTime(struct tm* tim)
{

	time_t current = time(NULL);
	time_t timestamp = mktime(tim);

	timestampOffset = timestamp - current;
    return RtcBspOK;
}

RtcBspError_e BSP_RtcGetCurrentDateTime(struct tm* datetime)
{
	time_t t = time(NULL);
	t += timestampOffset;
	*datetime = *localtime(&t);

    return RtcBspOK;
}

RtcBspError_e BSP_RtcGetCurrentTimestamp(time_t* timestamp)
{
	*timestamp = time(NULL) + timestampOffset;

    return RtcBspOK;
}

RtcBspError_e BSP_RtcSetAlarmOnDate(struct tm* datetime)
{
    if (datetime == NULL)
    {
        return RtcBspError;
    }

    return RtcBspOK;
}

RtcBspError_e BSP_RtcSetAlarmOnTimestamp(uint32_t secs)
{
    return RtcBspOK;
}

RtcBspError_e BSP_RtcSetAlarmInSecondsFromNow(uint32_t secs)
{
    return RtcBspOK;
}

RtcBspError_e BSP_RtcGetAlarmTimestamp(uint32_t* secs)
{
    if (secs == NULL)
    {
        return RtcBspError;
    }

    return RtcBspOK;
}

RtcBspError_e BSP_RtcEnableAlarmIrq()
{
    uint32_t cnt = 100000;

    if (cnt == 0)
    {
        return RtcBspError;
    }

    return RtcBspOK;
}
//TODO delete function if it will not be used in service
RtcBspError_e BSP_RtcDisableAlarmIrq()
{
    return RtcBspOK;
}
//TODO delete function if it will not be used in service
RtcBspError_e BSP_RtcMaskAlarmIrq()
{
    return RtcBspOK;
}
//TODO delete function if it will not be used in service
RtcBspError_e BSP_RtcUnmaskAlarmIrq()
{
    return RtcBspOK;
}

time_t BSP_RtcGetSecondCounter()
{
    time_t seconds = 0;
    time_t tmp = 0;
    do
    {

    } while (tmp != seconds);

    return seconds;
}

/**
 *      *******************************************************************************************************************************
 *      *                                                                                                                             *
 *      *                                      STATIC FUNCTIONS DEFINITIONS                                                           *
 *      *                                                                                                                             *
 *      *******************************************************************************************************************************
 */
//TODO delete function if it will not be used in service
void SNVS_HP_WRAPPER_IRQHandler()
{

}
/*
 *  **********************************************************************************************************************
 *  *                                                                                                                    *
 *  *                                                                                                                    *
 *  *                             SNVS RTC DRIVER STATIC FUNCTIONS COPIED FOR OUR USE                                    *
 *  *                                                                                                                    *
 *  *                                                                                                                    *
 *  **********************************************************************************************************************
 */
