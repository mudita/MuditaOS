// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "rtc_configuration.hpp"

#include <bsp/rtc/rtc.hpp>
#include <fsl_snvs_hp.h>
#include <fsl_snvs_lp.h>
#include <module-os/RTOSWrapper/include/ticks.hpp>
#include <time/time_constants.hpp>
#include <time.h>
#include <log/log.hpp>

namespace
{
    constexpr std::uint32_t irqTimeout = 100000;

    xQueueHandle qHandleRtcIrq = NULL;
    snvs_hp_rtc_config_t s_rtcConfig;
} // namespace

namespace bsp::rtc
{

    ErrorCode init(xQueueHandle qHandle)
    {
        qHandleRtcIrq = qHandle;
        CLOCK_EnableClock(kCLOCK_SnvsLp);
        SNVS_HP_RTC_GetDefaultConfig(&s_rtcConfig);

        s_rtcConfig.rtcCalValue  = getRtcCalibrationValue();
        s_rtcConfig.rtcCalEnable = true;
        SNVS_HP_RTC_Init(SNVS, &s_rtcConfig);

        SNVS_LPCR_LPTA_EN(1);

        bool timedOut     = false;
        auto timeoutTicks = cpp_freertos::Ticks::GetTicks() + pdMS_TO_TICKS(utils::time::milisecondsInSecond);
        const auto delay  = 10;

        SNVS->LPCR |= SNVS_LPCR_SRTC_ENV_MASK;
        while (!timedOut) {
            if ((SNVS->LPCR & SNVS_LPCR_SRTC_ENV_MASK)) {
                break;
            }
            timedOut = cpp_freertos::Ticks::GetTicks() > timeoutTicks;
            if (timedOut) {
                LOG_ERROR("RTC init timeout");
                return ErrorCode::Error;
            }
            vTaskDelay(delay);
        }

        SNVS_HP_RTC_TimeSynchronize(SNVS);

        NVIC_SetPriority(SNVS_HP_WRAPPER_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
        // Enable at the NVIC
        NVIC_EnableIRQ(SNVS_HP_WRAPPER_IRQn);

        // Start the timer
        SNVS_HP_RTC_StartTimer(SNVS);

        LOG_INFO("RTC configured successfully");
        return ErrorCode::OK;
    }

    ErrorCode setDateTimeFromTimestamp(time_t timestamp)
    {
        portENTER_CRITICAL();

        SNVS_LP_SRTC_StopTimer(SNVS);
        SNVS_LP_SRTC_SetSeconds(SNVS, static_cast<std::uint32_t>(timestamp));
        SNVS_LP_SRTC_StartTimer(SNVS);
        SNVS_HP_RTC_TimeSynchronize(SNVS);

        portEXIT_CRITICAL();

        return ErrorCode::OK;
    }

    ErrorCode setDateTime(struct tm *time)
    {
        if (time == NULL) {
            return ErrorCode::Error;
        }

        snvs_lp_srtc_datetime_t rtcDate;

        rtcDate.year   = time->tm_year + 1900;
        rtcDate.month  = time->tm_mon + 1;
        rtcDate.day    = time->tm_mday;
        rtcDate.hour   = time->tm_hour;
        rtcDate.minute = time->tm_min;
        rtcDate.second = time->tm_sec;

        portENTER_CRITICAL();
        SNVS_LP_SRTC_StopTimer(SNVS);
        SNVS_LP_SRTC_SetDatetime(SNVS, &rtcDate);
        SNVS_LP_SRTC_StartTimer(SNVS);
        SNVS_HP_RTC_TimeSynchronize(SNVS);
        portEXIT_CRITICAL();

        return ErrorCode::OK;
    }

    ErrorCode getCurrentDateTime(struct tm *datetime)
    {
        if (datetime == NULL) {
            return ErrorCode::Error;
        }

        snvs_hp_rtc_datetime_t rtcDate;

        SNVS_HP_RTC_GetDatetime(SNVS, &rtcDate);

        datetime->tm_year = rtcDate.year - 1900;
        datetime->tm_mon  = rtcDate.month - 1;
        datetime->tm_mday = rtcDate.day;
        datetime->tm_hour = rtcDate.hour;
        datetime->tm_min  = rtcDate.minute;
        datetime->tm_sec  = rtcDate.second;

        return ErrorCode::OK;
    }

    ErrorCode getCurrentTimestamp(time_t *timestamp)
    {
        if (timestamp == NULL) {
            return ErrorCode::Error;
        }

        *timestamp = static_cast<time_t>(SNVS_HP_RTC_GetSeconds(SNVS));

        return ErrorCode::OK;
    }

    ErrorCode setAlarmOnDate(struct tm *datetime)
    {
        if (datetime == NULL) {
            return ErrorCode::Error;
        }

        snvs_hp_rtc_datetime_t rtcDate;

        rtcDate.year   = datetime->tm_year + 1900;
        rtcDate.month  = datetime->tm_mon + 1;
        rtcDate.day    = datetime->tm_mday;
        rtcDate.hour   = datetime->tm_hour;
        rtcDate.minute = datetime->tm_min;
        rtcDate.second = datetime->tm_sec;

        enableAlarmIrq();
        SNVS_HP_RTC_SetAlarm(SNVS, &rtcDate);

        return ErrorCode::OK;
    }

    ErrorCode setAlarmOnTimestamp(std::uint32_t secs)
    {
        if (SNVS_HP_RTC_SetAlarmSeconds(SNVS, secs) != kStatus_Success) {
            return ErrorCode::Error;
        }

        enableAlarmIrq();

        return ErrorCode::OK;
    }

    ErrorCode setAlarmInSecondsFromNow(std::uint32_t secs)
    {
        std::uint32_t seconds = SNVS_HP_RTC_GetSeconds(SNVS);
        seconds += secs;

        enableAlarmIrq();

        if (SNVS_HP_RTC_SetAlarmSeconds(SNVS, seconds) != kStatus_Success) {
            return ErrorCode::Error;
        }

        return ErrorCode::OK;
    }

    ErrorCode getAlarmTimestamp(std::uint32_t *secs)
    {
        if (secs == NULL) {
            return ErrorCode::Error;
        }

        *secs = SNVS_HP_RTC_GetAlarmSeconds(SNVS);

        return ErrorCode::OK;
    }

    ErrorCode enableAlarmIrq()
    {
        std::uint32_t cnt = irqTimeout;
        SNVS->HPCR |= SNVS_HPCR_HPTA_EN_MASK;
        while ((!(SNVS->HPCR & SNVS_HPCR_HPTA_EN_MASK)) && cnt) {
            cnt--;
        }

        if (cnt == 0) {
            return ErrorCode::Error;
        }

        return ErrorCode::OK;
    }

    ErrorCode disableAlarmIrq()
    {
        std::uint32_t cnt = irqTimeout;

        SNVS->HPCR &= ~SNVS_HPCR_HPTA_EN_MASK;
        while ((SNVS->HPCR & SNVS_HPCR_HPTA_EN_MASK) && cnt) {
            cnt--;
        }

        if (cnt == 0) {
            return ErrorCode::Error;
        }

        return ErrorCode::OK;
    }

    ErrorCode maskAlarmIrq()
    {
        NVIC_DisableIRQ(SNVS_HP_WRAPPER_IRQn);
        return ErrorCode::OK;
    }

    ErrorCode unmaskAlarmIrq()
    {
        NVIC_EnableIRQ(SNVS_HP_WRAPPER_IRQn);
        return ErrorCode::OK;
    }

    ErrorCode setMinuteAlarm(time_t timestamp)
    {
        std::uint32_t secondsToMinute = 60 - (timestamp % 60);

        struct tm date;
        getCurrentDateTime(&date);

        return setAlarmInSecondsFromNow(secondsToMinute);
    }
} // namespace bsp::rtc

extern "C"
{
    void SNVS_HP_WRAPPER_IRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = 0;
        if (SNVS_HP_RTC_GetStatusFlags(SNVS) & kSNVS_RTC_AlarmInterruptFlag) {
            std::uint8_t notification = static_cast<std::uint8_t>(bsp::rtc::IrqNotification::AlarmOccurred);
            bsp::rtc::disableAlarmIrq();
            xQueueSendFromISR(qHandleRtcIrq, &notification, &xHigherPriorityTaskWoken);
            SNVS_HP_RTC_ClearStatusFlags(SNVS, kSNVS_RTC_AlarmInterruptFlag);
        }

        // Switch context if necessary
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

        // Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
        // exception return operation might vector to incorrect interrupt
#if defined __CORTEX_M && (__CORTEX_M == 4U)
        __DSB();
#endif
    }
}
