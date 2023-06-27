// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "rtc_configuration.hpp"
#include <bsp/rtc/rtc.hpp>
#include <fsl_snvs_hp.h>
#include <fsl_snvs_lp.h>
#include <ticks.hpp>
#include <critical.hpp>
#include <time/time_constants.hpp>
#include <time.h>
#include <log/log.hpp>

namespace
{
    constexpr std::uint32_t hpTimeAlarmIrqEnableTimeout = 100000;
    xQueueHandle qHandleRtcIrq                          = nullptr;

    bool isSrtcOperational()
    {
        constexpr auto timeoutMs    = utils::time::milisecondsInSecond;
        constexpr auto retryDelayMs = 10;
        const auto initialTicks     = cpp_freertos::Ticks::GetTicks();

        /* Wait until LP SRTC is operational */
        while ((SNVS->LPCR & SNVS_LPCR_SRTC_ENV_MASK) == 0) {
            if ((cpp_freertos::Ticks::GetTicks() - initialTicks) > cpp_freertos::Ticks::MsToTicks(timeoutMs)) {
                return false;
            }
            vTaskDelay(retryDelayMs);
        }
        return true;
    }

    void logRegisters()
    {
        LOG_INFO("SSM state: 0x%02lX", static_cast<std::uint32_t>(SNVS_HP_GetSSMState(SNVS)));
        LOG_INFO("LPCR: 0x%08lX LPLR: 0x%08lX LPSR: 0x%08lX", SNVS->LPCR, SNVS->LPLR, SNVS->LPSR);
        LOG_INFO("HPCR: 0x%08lX HPLR: 0x%08lX HPSR: 0x%08lX", SNVS->HPCR, SNVS->HPLR, SNVS->HPSR);
    }
} // namespace

namespace bsp::rtc
{
    ErrorCode init(xQueueHandle qHandle)
    {
        qHandleRtcIrq = qHandle;

        /* Enable HP and LP clocking */
        CLOCK_EnableClock(kCLOCK_SnvsHp);
        CLOCK_EnableClock(kCLOCK_SnvsLp);

        /* Init HP RTC */
        snvs_hp_rtc_config_t hpRtcConfig;
        SNVS_HP_RTC_GetDefaultConfig(&hpRtcConfig);
        hpRtcConfig.rtcCalValue  = getRtcCalibrationValue();
        hpRtcConfig.rtcCalEnable = true;
        SNVS_HP_RTC_Init(SNVS, &hpRtcConfig);

        /* Init LP SRTC */
        snvs_lp_srtc_config_t lpRtcConfig;
        SNVS_LP_SRTC_GetDefaultConfig(&lpRtcConfig);
        SNVS_LP_SRTC_Init(SNVS, &lpRtcConfig);

        if (enableLpSrtc() == ErrorCode::Error) {
            LOG_ERROR("LP SRTC init timeout!");
            return ErrorCode::Error;
        }

        SNVS_HP_RTC_TimeSynchronize(SNVS);

        NVIC_SetPriority(SNVS_HP_WRAPPER_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(SNVS_HP_WRAPPER_IRQn);

        /* Enable HP RTC */
        SNVS_HP_RTC_StartTimer(SNVS);

        printCurrentDataTime();

        LOG_INFO("RTC configured successfully");
        return ErrorCode::OK;
    }

    ErrorCode enableLpSrtc()
    {
        // Enable LP SRTC
        SNVS->LPCR |= SNVS_LPCR_SRTC_ENV_MASK;

        if (isSrtcOperational()) {
            return ErrorCode::OK;
        }

        // The LP SRTC doesn't start so reset LP registers and try again

        // Dump LP/HP registers
        logRegisters();

        // Reset the LP section except SRTC and Time alarm.
        SNVS_HP_ResetLP(SNVS);
        // Digital low-voltage detection is always enabled and cannot be disabled. At LP POR this
        // register is reset to all 0's, so the hardwired comparison fails and a low-voltage violation is
        // reported. Therefore, before programming any feature in the SNVS the low-voltage
        // violation should be cleared. The initialization software should write the proper value
        // (4173_6166h) into LPPGDR(LPLVDR) and should then clear
        // the low-voltage event record in the LP status register
        constexpr std::uint32_t LvdMagicNumber = 0x41736166;
        SNVS->LPPGDR                           = LvdMagicNumber;

        // Enable again LP SRTC after resetting the LP. It is mandatory!
        SNVS->LPCR |= SNVS_LPCR_SRTC_ENV_MASK;

        if (isSrtcOperational()) {
            LOG_INFO("LP SRTC reinit success!");
            return ErrorCode::OK;
        }

        // Resetting LP and LVD don't help
        // Dump registers and return an error
        logRegisters();

        return ErrorCode::Error;
    }

    void printCurrentDataTime()
    {
        struct tm datatime;
        getCurrentDateTime(&datatime);
        LOG_INFO("Startup RTC date: %04d/%02d/%02d time: %02d:%02d:%02d",
                 1900 + datatime.tm_year,
                 1 + datatime.tm_mon,
                 datatime.tm_mday,
                 datatime.tm_hour,
                 datatime.tm_min,
                 datatime.tm_sec);
    }

    ErrorCode setDateTimeFromTimestamp(time_t timestamp)
    {
        cpp_freertos::CriticalSection::Enter();
        SNVS_LP_SRTC_SetSeconds(SNVS, static_cast<std::uint32_t>(timestamp));
        SNVS_HP_RTC_TimeSynchronize(SNVS);
        cpp_freertos::CriticalSection::Exit();

        return ErrorCode::OK;
    }

    ErrorCode setDateTime(struct tm *time)
    {
        if (time == nullptr) {
            return ErrorCode::Error;
        }

        snvs_lp_srtc_datetime_t rtcDate;

        rtcDate.year   = time->tm_year + 1900;
        rtcDate.month  = time->tm_mon + 1;
        rtcDate.day    = time->tm_mday;
        rtcDate.hour   = time->tm_hour;
        rtcDate.minute = time->tm_min;
        rtcDate.second = time->tm_sec;

        cpp_freertos::CriticalSection::Enter();
        SNVS_LP_SRTC_SetDatetime(SNVS, &rtcDate);
        SNVS_HP_RTC_TimeSynchronize(SNVS);
        cpp_freertos::CriticalSection::Exit();

        return ErrorCode::OK;
    }

    ErrorCode getCurrentDateTime(struct tm *datetime)
    {
        if (datetime == nullptr) {
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
        if (timestamp == nullptr) {
            return ErrorCode::Error;
        }

        *timestamp = static_cast<std::time_t>(SNVS_HP_RTC_GetSeconds(SNVS));

        return ErrorCode::OK;
    }

    ErrorCode setAlarmOnDate(struct tm *datetime)
    {
        if (datetime == nullptr) {
            return ErrorCode::Error;
        }

        snvs_hp_rtc_datetime_t rtcDate;

        rtcDate.year   = datetime->tm_year + 1900;
        rtcDate.month  = datetime->tm_mon + 1;
        rtcDate.day    = datetime->tm_mday;
        rtcDate.hour   = datetime->tm_hour;
        rtcDate.minute = datetime->tm_min;
        rtcDate.second = datetime->tm_sec;

        if (SNVS_HP_RTC_SetAlarm(SNVS, &rtcDate) != kStatus_Success) {
            return ErrorCode::Error;
        }

        return enableAlarmIrq();
    }

    ErrorCode setAlarmOnTimestamp(std::uint32_t secs)
    {
        if (SNVS_HP_RTC_SetAlarmSeconds(SNVS, secs) != kStatus_Success) {
            return ErrorCode::Error;
        }

        return enableAlarmIrq();
    }

    ErrorCode setAlarmInSecondsFromNow(std::uint32_t secs)
    {
        std::uint32_t seconds = SNVS_HP_RTC_GetSeconds(SNVS);
        seconds += secs;

        if (SNVS_HP_RTC_SetAlarmSeconds(SNVS, seconds) != kStatus_Success) {
            return ErrorCode::Error;
        }

        return enableAlarmIrq();
    }

    ErrorCode getAlarmTimestamp(std::uint32_t *secs)
    {
        if (secs == nullptr) {
            return ErrorCode::Error;
        }

        *secs = SNVS_HP_RTC_GetAlarmSeconds(SNVS);

        return ErrorCode::OK;
    }

    ErrorCode enableAlarmIrq()
    {
        std::uint32_t timeout = hpTimeAlarmIrqEnableTimeout;

        SNVS->HPCR |= SNVS_HPCR_HPTA_EN_MASK;
        while ((SNVS->HPCR & SNVS_HPCR_HPTA_EN_MASK) == 0) {
            --timeout;
            if (timeout == 0) {
                LOG_ERROR("Failed to enable alarm IRQ!");
                return ErrorCode::Error;
            }
        }

        return ErrorCode::OK;
    }

    ErrorCode disableAlarmIrq()
    {
        std::uint32_t timeout = hpTimeAlarmIrqEnableTimeout;

        SNVS->HPCR &= ~SNVS_HPCR_HPTA_EN_MASK;
        while ((SNVS->HPCR & SNVS_HPCR_HPTA_EN_MASK) != 0) {
            --timeout;
            if (timeout == 0) {
                LOG_ERROR("Failed to disable alarm IRQ!");
                return ErrorCode::Error;
            }
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
        const auto secondsToMinute = 60 - (timestamp % 60);

        struct tm date
        {};
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
