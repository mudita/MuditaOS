/*
 * rtc.cpp
 *
 *  Created on: Jun 26, 2019
 *      Author: kuba
 */

#include "rtc/rtc.hpp"
#include "fsl_snvs_hp.h"
#include <time.h>

#include "FreeRTOS.h"

static xQueueHandle qHandleRtcIrq = NULL;

static  snvs_hp_rtc_config_t    s_rtcConfig;
static uint32_t SNVS_HP_ConvertDatetimeToSeconds(const snvs_hp_rtc_datetime_t *datetime);
static void SNVS_HP_ConvertSecondsToDatetime(uint32_t seconds, snvs_hp_rtc_datetime_t *datetime);
namespace bsp{

	RtcBspError_e rtc_Init(xQueueHandle qHandle)
	{
		qHandleRtcIrq = qHandle;

		SNVS_HP_RTC_GetDefaultConfig(&s_rtcConfig);
		SNVS_HP_RTC_Init(SNVS, &s_rtcConfig);

		NVIC_SetPriority(SNVS_HP_WRAPPER_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
		/* Enable at the NVIC */
		NVIC_EnableIRQ(SNVS_HP_WRAPPER_IRQn);

		// Start the timer
		SNVS_HP_RTC_StartTimer(SNVS);
		return RtcBspOK;
	}

	RtcBspError_e rtc_SetDateTimeFromTimestamp(time_t timestamp)
	{
		snvs_hp_rtc_datetime_t rtcDateTime;

		SNVS_HP_ConvertSecondsToDatetime((uint32_t)timestamp, &rtcDateTime);
		portENTER_CRITICAL();

		SNVS_HP_RTC_StopTimer(SNVS);
		SNVS_HP_RTC_SetDatetime(SNVS, &rtcDateTime);
		SNVS_HP_RTC_StartTimer(SNVS);

		portEXIT_CRITICAL();

		return RtcBspOK;
	}

	RtcBspError_e rtc_SetDateTime(struct tm* time)
	{
		if (time == NULL)
		{
			return RtcBspError;
		}

		snvs_hp_rtc_datetime_t rtcDate;

		rtcDate.year    = time->tm_year + 1900;
		rtcDate.month   = time->tm_mon  + 1;
		rtcDate.day     = time->tm_mday;
		rtcDate.hour    = time->tm_hour;
		rtcDate.minute  = time->tm_min;
		rtcDate.second  = time->tm_sec;

		portENTER_CRITICAL();
		SNVS_HP_RTC_StopTimer(SNVS);
		SNVS_HP_RTC_SetDatetime(SNVS, &rtcDate);
		SNVS_HP_RTC_StartTimer(SNVS);
		portEXIT_CRITICAL();

		return RtcBspOK;
	}

	RtcBspError_e rtc_GetCurrentDateTime(struct tm* datetime)
	{
		if (datetime == NULL)
		{
			return RtcBspError;
		}

		snvs_hp_rtc_datetime_t rtcDate;

		SNVS_HP_RTC_GetDatetime(SNVS, &rtcDate);

		datetime->tm_year  = rtcDate.year    - 1900;
		datetime->tm_mon   = rtcDate.month   - 1;
		datetime->tm_mday  = rtcDate.day;
		datetime->tm_hour  = rtcDate.hour;
		datetime->tm_min   = rtcDate.minute;
		datetime->tm_sec   = rtcDate.second;

		return RtcBspOK;
	}

	RtcBspError_e rtc_GetCurrentTimestamp(time_t* timestamp)
	{
		if (timestamp == NULL)
		{
			return RtcBspError;
		}
		snvs_hp_rtc_datetime_t rtcDate;

		SNVS_HP_RTC_GetDatetime(SNVS, &rtcDate);
		*timestamp = (time_t)SNVS_HP_ConvertDatetimeToSeconds(&rtcDate);

		return RtcBspOK;
	}

	RtcBspError_e rtc_SetAlarmOnDate(struct tm* datetime)
	{
		if (datetime == NULL)
		{
			return RtcBspError;
		}

		snvs_hp_rtc_datetime_t rtcDate;

		rtcDate.year    = datetime->tm_year + 1900;
		rtcDate.month   = datetime->tm_mon  + 1;
		rtcDate.day     = datetime->tm_mday;
		rtcDate.hour    = datetime->tm_hour;
		rtcDate.minute  = datetime->tm_min;
		rtcDate.second  = datetime->tm_sec;

		rtc_EnableAlarmIrq();
		SNVS_HP_RTC_SetAlarm(SNVS, &rtcDate);

		return RtcBspOK;
	}

	RtcBspError_e rtc_SetAlarmOnTimestamp(uint32_t secs)
	{
		snvs_hp_rtc_datetime_t rtcDate;

		SNVS_HP_ConvertSecondsToDatetime(secs, &rtcDate);

		if (SNVS_HP_RTC_SetAlarm(SNVS, &rtcDate) != kStatus_Success)
		{
			return RtcBspError;
		}

		rtc_EnableAlarmIrq();

		return RtcBspOK;
	}

	RtcBspError_e rtc_SetAlarmInSecondsFromNow(uint32_t secs)
	{
		snvs_hp_rtc_datetime_t rtcDate;

		SNVS_HP_RTC_GetDatetime(SNVS, &rtcDate);

		uint32_t seconds = SNVS_HP_ConvertDatetimeToSeconds(&rtcDate);

		seconds += secs;

		SNVS_HP_ConvertSecondsToDatetime(seconds, &rtcDate);

		rtc_EnableAlarmIrq();
		SNVS_HP_RTC_SetAlarm(SNVS, &rtcDate);

		return RtcBspOK;
	}

	RtcBspError_e rtc_GetAlarmTimestamp(uint32_t* secs)
	{
		if (secs == NULL)
		{
			return RtcBspError;
		}

		snvs_hp_rtc_datetime_t rtcDate;

		SNVS_HP_RTC_GetAlarm(SNVS, &rtcDate);

		*secs = SNVS_HP_ConvertDatetimeToSeconds(&rtcDate);
		return RtcBspOK;
	}
	static const uint32_t irqTimeout = 100000;
	RtcBspError_e rtc_EnableAlarmIrq()
	{
		uint32_t cnt = irqTimeout;
		SNVS->HPCR |= SNVS_HPCR_HPTA_EN_MASK;
		while ((!(SNVS->HPCR & SNVS_HPCR_HPTA_EN_MASK)) && cnt)
		{
			cnt--;
		}

		if (cnt == 0)
		{
			return RtcBspError;
		}

		return RtcBspOK;
	}

	RtcBspError_e rtc_DisableAlarmIrq()
	{
		uint32_t cnt = irqTimeout;

		SNVS->HPCR &= ~SNVS_HPCR_HPTA_EN_MASK;
		while ((SNVS->HPCR & SNVS_HPCR_HPTA_EN_MASK) && cnt)
		{
			cnt--;
		}

		if (cnt == 0)
		{
			return RtcBspError;
		}

		return RtcBspOK;
	}

	RtcBspError_e rtc_MaskAlarmIrq()
	{
		NVIC_DisableIRQ(SNVS_HP_WRAPPER_IRQn);
		return RtcBspOK;
	}

	RtcBspError_e rtc_UnmaskAlarmIrq()
	{
		NVIC_EnableIRQ(SNVS_HP_WRAPPER_IRQn);
		return RtcBspOK;
	}

	time_t rtc_GetSecondCounter()
	{
		time_t seconds = 0;
		time_t tmp = 0;

		do
		{
			seconds = tmp;
			tmp = (SNVS->HPRTCMR << 17U) | (SNVS->HPRTCLR >> 15U);
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

		return rtc_SetAlarmInSecondsFromNow(secondsToMinute);
	}
}



extern "C"
{
	void SNVS_HP_WRAPPER_IRQHandler()
	{
		BaseType_t xHigherPriorityTaskWoken = 0;
		if (SNVS_HP_RTC_GetStatusFlags(SNVS) & kSNVS_RTC_AlarmInterruptFlag)
		{
			uint8_t notification = static_cast<uint8_t>(bsp::rtcIrqNotifications::alarmOcured);
			bsp::rtc_DisableAlarmIrq();
			xQueueSendFromISR(qHandleRtcIrq,&notification, &xHigherPriorityTaskWoken);
			//TODO service function call
		  //  RtcAlarmIrqHandler();
			/* Clear alarm flag */
			SNVS_HP_RTC_ClearStatusFlags(SNVS, kSNVS_RTC_AlarmInterruptFlag);
		}
		 // Switch context if necessary
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
		  exception return operation might vector to incorrect interrupt */
		#if defined __CORTEX_M && (__CORTEX_M == 4U)
			__DSB();
		#endif
	}

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
static const uint32_t SECONDS_IN_A_DAY = 86400;
static const uint32_t SECONDS_IN_A_HOUR = 3600;
static const uint32_t SECONDS_IN_A_MINUTE = 60;
static const uint32_t DAYS_IN_A_YEAR = 365;
static const uint32_t YEAR_RANGE_START = 1970;

static uint32_t SNVS_HP_ConvertDatetimeToSeconds(const snvs_hp_rtc_datetime_t *datetime)
{
    assert(datetime);

    /* Number of days from begin of the non Leap-year*/
    /* Number of days from begin of the non Leap-year*/
    uint16_t monthDays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
    uint32_t seconds;

    /* Compute number of days from 1970 till given year*/
    seconds = (datetime->year - 1970U) * DAYS_IN_A_YEAR;
    /* Add leap year days */
    seconds += ((datetime->year / 4) - (1970U / 4));
    /* Add number of days till given month*/
    seconds += monthDays[datetime->month];
    /* Add days in given month. We subtract the current day as it is
     * represented in the hours, minutes and seconds field*/
    seconds += (datetime->day - 1);
    /* For leap year if month less than or equal to Febraury, decrement day counter*/
    if ((!(datetime->year & 3U)) && (datetime->month <= 2U))
    {
        seconds--;
    }

    seconds = (seconds * SECONDS_IN_A_DAY) + (datetime->hour * SECONDS_IN_A_HOUR) +
              (datetime->minute * SECONDS_IN_A_MINUTE) + datetime->second;

    return seconds;
}

static void SNVS_HP_ConvertSecondsToDatetime(uint32_t seconds, snvs_hp_rtc_datetime_t *datetime)
{
    assert(datetime);

    uint32_t x;
    uint32_t secondsRemaining, days;
    uint16_t daysInYear;
    /* Table of days in a month for a non leap year. First entry in the table is not used,
     * valid months start from 1
     */
    uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    /* Start with the seconds value that is passed in to be converted to date time format */
    secondsRemaining = seconds;

    /* Calcuate the number of days, we add 1 for the current day which is represented in the
     * hours and seconds field
     */
    days = secondsRemaining / SECONDS_IN_A_DAY + 1;

    /* Update seconds left*/
    secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY;

    /* Calculate the datetime hour, minute and second fields */
    datetime->hour = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    datetime->minute = secondsRemaining / 60U;
    datetime->second = secondsRemaining % SECONDS_IN_A_MINUTE;

    /* Calculate year */
    daysInYear = DAYS_IN_A_YEAR;
    datetime->year = YEAR_RANGE_START;
    while (days > daysInYear)
    {
        /* Decrease day count by a year and increment year by 1 */
        days -= daysInYear;
        datetime->year++;

        /* Adjust the number of days for a leap year */
        if (datetime->year & 3U)
        {
            daysInYear = DAYS_IN_A_YEAR;
        }
        else
        {
            daysInYear = DAYS_IN_A_YEAR + 1;
        }
    }

    /* Adjust the days in February for a leap year */
    if (!(datetime->year & 3U))
    {
        daysPerMonth[2] = 29U;
    }

    for (x = 1U; x <= 12U; x++)
    {
        if (days <= daysPerMonth[x])
        {
            datetime->month = x;
            break;
        }
        else
        {
            days -= daysPerMonth[x];
        }
    }

    datetime->day = days;
}
