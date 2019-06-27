/*
 * rtc.cpp
 *
 *  Created on: Jun 26, 2019
 *      Author: kuba
 */

#include "rtc/rtc.hpp"

#include <time.h>

#include "FreeRTOS.h"



//static uint32_t SNVS_HP_ConvertDatetimeToSeconds(const snvs_hp_rtc_datetime_t *datetime);

//static void SNVS_HP_ConvertSecondsToDatetime(uint32_t seconds, snvs_hp_rtc_datetime_t *datetime);

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

RtcBspError_e BSP_RtcDisableAlarmIrq()
{
    uint32_t cnt = 100000;



    if (cnt == 0)
    {
        return RtcBspError;
    }

    return RtcBspOK;
}

RtcBspError_e BSP_RtcMaskAlarmIrq()
{

    return RtcBspOK;
}

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

#define SECONDS_IN_A_DAY (86400U)
#define SECONDS_IN_A_HOUR (3600U)
#define SECONDS_IN_A_MINUTE (60U)
#define DAYS_IN_A_YEAR (365U)
#define YEAR_RANGE_START (1970U)
#define YEAR_RANGE_END (2099U)
/*
static uint32_t SNVS_HP_ConvertDatetimeToSeconds(const snvs_hp_rtc_datetime_t *datetime)
{
    assert(datetime);

     Number of days from begin of the non Leap-year
     Number of days from begin of the non Leap-year
    uint16_t monthDays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
    uint32_t seconds;

     Compute number of days from 1970 till given year
    seconds = (datetime->year - 1970U) * DAYS_IN_A_YEAR;
     Add leap year days
    seconds += ((datetime->year / 4) - (1970U / 4));
     Add number of days till given month
    seconds += monthDays[datetime->month];
     Add days in given month. We subtract the current day as it is
     * represented in the hours, minutes and seconds field
    seconds += (datetime->day - 1);
     For leap year if month less than or equal to Febraury, decrement day counter
    if ((!(datetime->year & 3U)) && (datetime->month <= 2U))
    {
        seconds--;
    }

    seconds = (seconds * SECONDS_IN_A_DAY) + (datetime->hour * SECONDS_IN_A_HOUR) +
              (datetime->minute * SECONDS_IN_A_MINUTE) + datetime->second;

    return seconds;
}*/
/*

static void SNVS_HP_ConvertSecondsToDatetime(uint32_t seconds, snvs_hp_rtc_datetime_t *datetime)
{
    assert(datetime);

    uint32_t x;
    uint32_t secondsRemaining, days;
    uint16_t daysInYear;
     Table of days in a month for a non leap year. First entry in the table is not used,
     * valid months start from 1

    uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

     Start with the seconds value that is passed in to be converted to date time format
    secondsRemaining = seconds;

     Calcuate the number of days, we add 1 for the current day which is represented in the
     * hours and seconds field

    days = secondsRemaining / SECONDS_IN_A_DAY + 1;

     Update seconds left
    secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY;

     Calculate the datetime hour, minute and second fields
    datetime->hour = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    datetime->minute = secondsRemaining / 60U;
    datetime->second = secondsRemaining % SECONDS_IN_A_MINUTE;

     Calculate year
    daysInYear = DAYS_IN_A_YEAR;
    datetime->year = YEAR_RANGE_START;
    while (days > daysInYear)
    {
         Decrease day count by a year and increment year by 1
        days -= daysInYear;
        datetime->year++;

         Adjust the number of days for a leap year
        if (datetime->year & 3U)
        {
            daysInYear = DAYS_IN_A_YEAR;
        }
        else
        {
            daysInYear = DAYS_IN_A_YEAR + 1;
        }
    }

     Adjust the days in February for a leap year
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
*/


