/*
 * rtc.hpp
 *
 *  Created on: Jun 26, 2019
 *      Author: kuba
 */

#ifndef MODULE_BSP_BSP_RTC_RTC_HPP_
#define MODULE_BSP_BSP_RTC_RTC_HPP_


#include <time.h>
#include <stdint.h>

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
}

typedef enum
{
    RtcBspOK,
    RtcBspError
} RtcBspError_e;


namespace bsp {

	enum class rtcIrqNotifications
	{
		alarmOcured = 0x01
	};

	/*
	 * RTC functions are using FreeRTOS functions, RTC should be initialized after scheduler starts.
	 * */
	RtcBspError_e rtc_Init(xQueueHandle qHandle);

	RtcBspError_e rtc_EnableAlarmIrq();

	RtcBspError_e rtc_DisableAlarmIrq();

	RtcBspError_e rtc_MaskAlarmIrq();

	RtcBspError_e rtc_UnmaskAlarmIrq();

	RtcBspError_e rtc_SetDateTimeFromTimestamp(time_t timestamp);

	RtcBspError_e rtc_SetDateTime(struct tm* time);

	RtcBspError_e rtc_GetCurrentDateTime(struct tm* datetime);

	RtcBspError_e rtc_GetCurrentTimestamp(time_t* timestamp);

	RtcBspError_e rtc_SetAlarmOnDate(struct tm* datetime);

	RtcBspError_e rtc_SetAlarmOnTimestamp(uint32_t secs);

	RtcBspError_e rtc_SetAlarmInSecondsFromNow(uint32_t secs);

	RtcBspError_e rtc_GetAlarmTimestamp(uint32_t* secs);

	time_t rtc_GetSecondCounter();

}
#endif /* MODULE_BSP_BSP_RTC_RTC_HPP_ */
