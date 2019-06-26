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

typedef enum
{
    RtcBspOK,
    RtcBspError
} RtcBspError_e;

RtcBspError_e BSP_RtcInit();

RtcBspError_e BSP_RtcEnableAlarmIrq();

RtcBspError_e BSP_RtcDisableAlarmIrq();

RtcBspError_e BSP_RtcMaskAlarmIrq();

RtcBspError_e BSP_RtcUnmaskAlarmIrq();

RtcBspError_e BSP_RtcSetDateTimeFromTimestamp(time_t timestamp);

RtcBspError_e BSP_RtcSetDateTime(struct tm* time);

RtcBspError_e BSP_RtcGetCurrentDateTime(struct tm* datetime);

RtcBspError_e BSP_RtcGetCurrentTimestamp(time_t* timestamp);

RtcBspError_e BSP_RtcSetAlarmOnDate(struct tm* datetime);

RtcBspError_e BSP_RtcSetAlarmOnTimestamp(uint32_t secs);

RtcBspError_e BSP_RtcSetAlarmInSecondsFromNow(uint32_t secs);

RtcBspError_e BSP_RtcGetAlarmTimestamp(uint32_t* secs);

time_t BSP_RtcGetSecondCounter();


#endif /* MODULE_BSP_BSP_RTC_RTC_HPP_ */
