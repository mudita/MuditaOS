/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017, NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_snvs_hp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.snvs_hp"
#endif

#define SECONDS_IN_A_DAY    (86400U)
#define SECONDS_IN_A_HOUR   (3600U)
#define SECONDS_IN_A_MINUTE (60U)
#define DAYS_IN_A_YEAR      (365U)
#define YEAR_RANGE_START    (1970U)
#define YEAR_RANGE_END      (2099U)

#if !(defined(SNVS_HPCOMR_SW_SV_MASK))
#define SNVS_HPCOMR_SW_SV_MASK (0x100U)
#endif
#if !(defined(SNVS_HPSR_PI_MASK))
#define SNVS_HPSR_PI_MASK (0x2U)
#endif
#if !(defined(SNVS_HPSR_HPTA_MASK))
#define SNVS_HPSR_HPTA_MASK (0x1U)
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Checks whether the date and time passed in is valid
 *
 * @param datetime Pointer to structure where the date and time details are stored
 *
 * @return Returns false if the date & time details are out of range; true if in range
 */
static bool SNVS_HP_CheckDatetimeFormat(const snvs_hp_rtc_datetime_t *datetime);

/*!
 * @brief Converts time data from datetime to seconds
 *
 * @param datetime Pointer to datetime structure where the date and time details are stored
 *
 * @return The result of the conversion in seconds
 */
static uint32_t SNVS_HP_ConvertDatetimeToSeconds(const snvs_hp_rtc_datetime_t *datetime);

/*!
 * @brief Converts time data from seconds to a datetime structure
 *
 * @param seconds  Seconds value that needs to be converted to datetime format
 * @param datetime Pointer to the datetime structure where the result of the conversion is stored
 */
static void SNVS_HP_ConvertSecondsToDatetime(uint32_t seconds, snvs_hp_rtc_datetime_t *datetime);

/*!
 * @brief Returns RTC time in seconds.
 *
 * This function is used internally to get actual RTC time in seconds.
 *
 * @param base SNVS peripheral base address
 *
 * @return RTC time in seconds
 */
static uint32_t SNVS_HP_RTC_GetSeconds(SNVS_Type *base);

#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) &&                       \
     defined(SNVS_HP_CLOCKS))
/*!
 * @brief Get the SNVS instance from peripheral base address.
 *
 * @param base SNVS peripheral base address.
 *
 * @return SNVS instance.
 */
static uint32_t SNVS_HP_GetInstance(SNVS_Type *base);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Variables
 ******************************************************************************/
#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) &&                       \
     defined(SNVS_HP_CLOCKS))
/*! @brief Pointer to snvs_hp clock. */
const clock_ip_name_t s_snvsHpClock[] = SNVS_HP_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static bool SNVS_HP_CheckDatetimeFormat(const snvs_hp_rtc_datetime_t *datetime)
{
    assert(datetime);

    /* Table of days in a month for a non leap year. First entry in the table is not used,
     * valid months start from 1
     */
    uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    /* Check year, month, hour, minute, seconds */
    if ((datetime->year < YEAR_RANGE_START) || (datetime->year > YEAR_RANGE_END) || (datetime->month > 12U) ||
        (datetime->month < 1U) || (datetime->hour >= 24U) || (datetime->minute >= 60U) || (datetime->second >= 60U)) {
        /* If not correct then error*/
        return false;
    }

    /* Adjust the days in February for a leap year */
    if ((((datetime->year & 3U) == 0) && (datetime->year % 100 != 0)) || (datetime->year % 400 == 0)) {
        daysPerMonth[2] = 29U;
    }

    /* Check the validity of the day */
    if ((datetime->day > daysPerMonth[datetime->month]) || (datetime->day < 1U)) {
        return false;
    }

    return true;
}

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
    if ((!(datetime->year & 3U)) && (datetime->month <= 2U)) {
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
    datetime->hour   = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    datetime->minute = secondsRemaining / 60U;
    datetime->second = secondsRemaining % SECONDS_IN_A_MINUTE;

    /* Calculate year */
    daysInYear     = DAYS_IN_A_YEAR;
    datetime->year = YEAR_RANGE_START;
    while (days > daysInYear) {
        /* Decrease day count by a year and increment year by 1 */
        days -= daysInYear;
        datetime->year++;

        /* Adjust the number of days for a leap year */
        if (datetime->year & 3U) {
            daysInYear = DAYS_IN_A_YEAR;
        }
        else {
            daysInYear = DAYS_IN_A_YEAR + 1;
        }
    }

    /* Adjust the days in February for a leap year */
    if (!(datetime->year & 3U)) {
        daysPerMonth[2] = 29U;
    }

    for (x = 1U; x <= 12U; x++) {
        if (days <= daysPerMonth[x]) {
            datetime->month = x;
            break;
        }
        else {
            days -= daysPerMonth[x];
        }
    }

    datetime->day = days;
}

#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) &&                       \
     defined(SNVS_HP_CLOCKS))
static uint32_t SNVS_HP_GetInstance(SNVS_Type *base)
{
    return 0U;
}
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

void SNVS_HP_RTC_Init(SNVS_Type *base, const snvs_hp_rtc_config_t *config)
{
    assert(config);

#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) &&                       \
     defined(SNVS_HP_CLOCKS))
    uint32_t instance = SNVS_HP_GetInstance(base);
    CLOCK_EnableClock(s_snvsHpClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    base->HPCOMR |= SNVS_HPCOMR_NPSWA_EN_MASK | SNVS_HPCOMR_SW_SV_MASK;

    base->HPCR = SNVS_HPCR_PI_FREQ(config->periodicInterruptFreq);

    if (config->rtcCalEnable) {
        base->HPCR = SNVS_HPCR_HPCALB_VAL_MASK & (config->rtcCalValue << SNVS_HPCR_HPCALB_VAL_SHIFT);
        base->HPCR |= SNVS_HPCR_HPCALB_EN_MASK;
    }
}

void SNVS_HP_RTC_Deinit(SNVS_Type *base)
{
    base->HPCR &= ~SNVS_HPCR_RTC_EN_MASK;

#if (!(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) &&                       \
     defined(SNVS_HP_CLOCKS))
    uint32_t instance = SNVS_HP_GetInstance(base);
    CLOCK_DisableClock(s_snvsHpClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

void SNVS_HP_RTC_GetDefaultConfig(snvs_hp_rtc_config_t *config)
{
    assert(config);

    config->rtcCalEnable          = false;
    config->rtcCalValue           = 0U;
    config->periodicInterruptFreq = 0U;
}

static uint32_t SNVS_HP_RTC_GetSeconds(SNVS_Type *base)
{
    uint32_t seconds = 0;
    uint32_t tmp     = 0;

    /* Do consecutive reads until value is correct */
    do {
        seconds = tmp;
        tmp     = (base->HPRTCMR << 17U) | (base->HPRTCLR >> 15U);
    } while (tmp != seconds);

    return seconds;
}

status_t SNVS_HP_RTC_SetDatetime(SNVS_Type *base, const snvs_hp_rtc_datetime_t *datetime)
{
    assert(datetime);

    uint32_t seconds = 0U;
    uint32_t tmp     = base->HPCR;

    /* disable RTC */
    SNVS_HP_RTC_StopTimer(base);

    /* Return error if the time provided is not valid */
    if (!(SNVS_HP_CheckDatetimeFormat(datetime))) {
        return kStatus_InvalidArgument;
    }

    /* Set time in seconds */
    seconds = SNVS_HP_ConvertDatetimeToSeconds(datetime);

    base->HPRTCMR = (uint32_t)(seconds >> 17U);
    base->HPRTCLR = (uint32_t)(seconds << 15U);

    /* reenable RTC in case that it was enabled before */
    if (tmp & SNVS_HPCR_RTC_EN_MASK) {
        SNVS_HP_RTC_StartTimer(base);
    }

    return kStatus_Success;
}

void SNVS_HP_RTC_GetDatetime(SNVS_Type *base, snvs_hp_rtc_datetime_t *datetime)
{
    assert(datetime);

    SNVS_HP_ConvertSecondsToDatetime(SNVS_HP_RTC_GetSeconds(base), datetime);
}

status_t SNVS_HP_RTC_SetAlarm(SNVS_Type *base, const snvs_hp_rtc_datetime_t *alarmTime)
{
    assert(alarmTime);

    uint32_t alarmSeconds = 0U;
    uint32_t currSeconds  = 0U;
    uint32_t tmp          = base->HPCR;

    /* Return error if the alarm time provided is not valid */
    if (!(SNVS_HP_CheckDatetimeFormat(alarmTime))) {
        return kStatus_InvalidArgument;
    }

    alarmSeconds = SNVS_HP_ConvertDatetimeToSeconds(alarmTime);
    currSeconds  = SNVS_HP_RTC_GetSeconds(base);

    /* Return error if the alarm time has passed */
    if (alarmSeconds < currSeconds) {
        return kStatus_Fail;
    }

    /* disable RTC alarm interrupt */
    base->HPCR &= ~SNVS_HPCR_HPTA_EN_MASK;
    while (base->HPCR & SNVS_HPCR_HPTA_EN_MASK) {}

    /* Set alarm in seconds*/
    base->HPTAMR = (uint32_t)(alarmSeconds >> 17U);
    base->HPTALR = (uint32_t)(alarmSeconds << 15U);

    /* reenable RTC alarm interrupt in case that it was enabled before */
    base->HPCR = tmp;

    return kStatus_Success;
}

void SNVS_HP_RTC_GetAlarm(SNVS_Type *base, snvs_hp_rtc_datetime_t *datetime)
{
    assert(datetime);

    uint32_t alarmSeconds = 0U;

    /* Get alarm in seconds  */
    alarmSeconds = (base->HPTAMR << 17U) | (base->HPTALR >> 15U);

    SNVS_HP_ConvertSecondsToDatetime(alarmSeconds, datetime);
}

#if (defined(FSL_FEATURE_SNVS_HAS_SRTC) && (FSL_FEATURE_SNVS_HAS_SRTC > 0))
void SNVS_HP_RTC_TimeSynchronize(SNVS_Type *base)
{
    uint32_t tmp = base->HPCR;

    /* disable RTC */
    SNVS_HP_RTC_StopTimer(base);

    base->HPCR |= SNVS_HPCR_HP_TS_MASK;

    /* reenable RTC in case that it was enabled before */
    if (tmp & SNVS_HPCR_RTC_EN_MASK) {
        SNVS_HP_RTC_StartTimer(base);
    }
}
#endif /* FSL_FEATURE_SNVS_HAS_SRTC */

uint32_t SNVS_HP_RTC_GetStatusFlags(SNVS_Type *base)
{
    uint32_t flags = 0U;

    if (base->HPSR & SNVS_HPSR_PI_MASK) {
        flags |= kSNVS_RTC_PeriodicInterruptFlag;
    }

    if (base->HPSR & SNVS_HPSR_HPTA_MASK) {
        flags |= kSNVS_RTC_AlarmInterruptFlag;
    }

    return flags;
}

uint32_t SNVS_HP_RTC_GetEnabledInterrupts(SNVS_Type *base)
{
    uint32_t val = 0U;

    if (base->HPCR & SNVS_HPCR_PI_EN_MASK) {
        val |= kSNVS_RTC_PeriodicInterrupt;
    }

    if (base->HPCR & SNVS_HPCR_HPTA_EN_MASK) {
        val |= kSNVS_RTC_AlarmInterrupt;
    }

    return val;
}
