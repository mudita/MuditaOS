/*
 * rtc.cpp
 *
 *  Created on: Jun 26, 2019
 *      Author: kuba
 */

#include "rtc/rtc.hpp"
#include <time.h>

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
}


static time_t timestampOffset;
static time_t timestampAlarm;
time_t localOffset;
static xQueueHandle qHandleRtcIrq = NULL;
static TaskHandle_t rtcWorkerHandle = NULL;

static void rtc_worker(void *pvp);

namespace bsp{

	RtcBspError_e rtc_Init(xQueueHandle qHandle)
	{
		qHandleRtcIrq = qHandle;
		timestampOffset = 0;
		timestampAlarm = 0;

		xTaskCreate(rtc_worker, "rtc_worker", 512, qHandle, 0, &rtcWorkerHandle);
		return RtcBspOK;
	}

	RtcBspError_e rtc_SetDateTimeFromTimestamp(time_t timestamp)
	{

		time_t current = time(NULL);
		struct tm* local = localtime(&current);

		localOffset = local->tm_gmtoff;

		timestampOffset = timestamp - current;
		return RtcBspOK;
	}

	RtcBspError_e rtc_SetDateTime(struct tm* tim)
	{

		time_t current = time(NULL);
		time_t timestamp = mktime(tim);

		timestampOffset = timestamp - current;
		return RtcBspOK;
	}

	RtcBspError_e rtc_GetCurrentDateTime(struct tm* datetime)
	{
		time_t t = time(NULL);
		t += timestampOffset;
		*datetime = *localtime(&t);

		return RtcBspOK;
	}

	RtcBspError_e rtc_GetCurrentTimestamp(time_t* timestamp)
	{
		*timestamp = time(NULL) + timestampOffset;

		return RtcBspOK;
	}

	RtcBspError_e rtc_SetAlarmOnDate(struct tm* datetime)
	{
		if (datetime == NULL)
		{
			return RtcBspError;
		}

		return RtcBspOK;
	}

	RtcBspError_e rtc_SetAlarmOnTimestamp(uint32_t secs)
	{
		timestampAlarm = secs ;
		return RtcBspOK;
	}

	RtcBspError_e rtc_SetAlarmInSecondsFromNow(uint32_t secs)
	{
		time_t current;
		if(	rtc_GetCurrentTimestamp(&current) != RtcBspOK )
			return RtcBspError;

		current += secs ;

		if(	rtc_SetAlarmOnTimestamp(current)!= RtcBspOK )
			return RtcBspError;

		return RtcBspOK;
	}

	RtcBspError_e rtc_GetAlarmTimestamp(uint32_t* secs)
	{
		if (secs == NULL)
		{
			return RtcBspError;
		}

		return RtcBspOK;
	}

	RtcBspError_e rtc_EnableAlarmIrq()
	{
		uint32_t cnt = 100000;

		if (cnt == 0)
		{
			return RtcBspError;
		}

		return RtcBspOK;
	}
	//TODO delete function if it will not be used in service
	RtcBspError_e rtc_DisableAlarmIrq()
	{
		return RtcBspOK;
	}
	//TODO delete function if it will not be used in service
	RtcBspError_e zrtc_MaskAlarmIrq()
	{
		return RtcBspOK;
	}
	//TODO delete function if it will not be used in service
	RtcBspError_e rtc_UnmaskAlarmIrq()
	{
		return RtcBspOK;
	}

	time_t rtc_GetSecondCounter()
	{
		time_t seconds = 0;
		time_t tmp = 0;
		do
		{

		} while (tmp != seconds);

		return seconds;
	}

	RtcBspError_e rtc_SetMinuteAlarm(time_t timestamp)
	{
		uint32_t secondsToMinute = 60 - ( timestamp % 60 );

		struct tm date;
		rtc_GetCurrentDateTime(&date);

/*		LOG_INFO("seconds %d",  ( timestamp % 60 ));
		LOG_INFO("seconds to minute %d", secondsToMinute);*/

		rtc_SetAlarmInSecondsFromNow(secondsToMinute);
	}
}

static void rtc_worker(void *pvp)
{
	while(1)
	{
		time_t current;
		bsp::rtc_GetCurrentTimestamp(&current);

		uint8_t notification;
		if(current == timestampAlarm)
		{
			notification = static_cast<uint8_t>(bsp::rtcIrqNotifications::alarmOcured);
			xQueueSend(qHandleRtcIrq, &notification, 100);
		}
		vTaskDelay(1000);
	}
}

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
