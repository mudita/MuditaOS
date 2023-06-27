// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/rtc/rtc.hpp>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <time.h>
#include <log/log.hpp>

namespace
{
    time_t timestampOffset;
    time_t timestampAlarm;
    time_t localOffset;
    xQueueHandle qHandleRtcIrq         = nullptr;
    TaskHandle_t rtcWorkerHandle       = nullptr;
    constexpr std::uint16_t stackDepth = 512;

    void rtcWorker(void *pvp)
    {
        while (true) {
            time_t current;
            bsp::rtc::getCurrentTimestamp(&current);

            std::uint8_t notification;
            if (current == timestampAlarm) {
                notification = static_cast<std::uint8_t>(bsp::rtc::IrqNotification::AlarmOccurred);
                xQueueSend(qHandleRtcIrq, &notification, 100);
            }
            vTaskDelay(1000);
        }
    }
} // namespace

namespace bsp::rtc
{
    ErrorCode init(xQueueHandle qHandle)
    {
        qHandleRtcIrq   = qHandle;
        timestampOffset = 0;
        timestampAlarm  = 0;

        xTaskCreate(rtcWorker, "rtc_worker", stackDepth, qHandle, 0, &rtcWorkerHandle);
        printCurrentDataTime();
        return ErrorCode::OK;
    }

    ErrorCode enableLpSrtc()
    {
        return ErrorCode::OK;
    }

    void printCurrentDataTime()
    {
        struct tm datatime;
        getCurrentDateTime(&datatime);
        LOG_INFO("Startup date: %04d/%02d/%02d time: %02d:%02d:%02d",
                 1900 + datatime.tm_year,
                 1 + datatime.tm_mon,
                 datatime.tm_mday,
                 datatime.tm_hour,
                 datatime.tm_min,
                 datatime.tm_sec);
    }

    ErrorCode setDateTimeFromTimestamp(time_t timestamp)
    {
        time_t current   = time(nullptr);
        struct tm *local = localtime(&current);

        localOffset = local->tm_gmtoff;

        timestampOffset = timestamp - current;
        return ErrorCode::OK;
    }

    ErrorCode setDateTime(struct tm *tim)
    {

        time_t current   = time(nullptr);
        time_t timestamp = mktime(tim);

        timestampOffset = timestamp - current;
        return ErrorCode::OK;
    }

    ErrorCode getCurrentDateTime(struct tm *datetime)
    {
        time_t t = time(nullptr);
        t += timestampOffset;
        *datetime = *gmtime(&t);

        return ErrorCode::OK;
    }

    ErrorCode getCurrentTimestamp(time_t *timestamp)
    {
        *timestamp = time(nullptr) + timestampOffset;

        return ErrorCode::OK;
    }

    ErrorCode setAlarmOnDate(struct tm *datetime)
    {
        if (datetime == nullptr) {
            return ErrorCode::Error;
        }

        return ErrorCode::OK;
    }

    ErrorCode setAlarmOnTimestamp(std::uint32_t secs)
    {
        timestampAlarm = secs;
        return ErrorCode::OK;
    }

    ErrorCode setAlarmInSecondsFromNow(std::uint32_t secs)
    {
        time_t current;
        if (getCurrentTimestamp(&current) != ErrorCode::OK)
            return ErrorCode::Error;

        current += secs;

        if (setAlarmOnTimestamp(current) != ErrorCode::OK)
            return ErrorCode::Error;

        return ErrorCode::OK;
    }

    ErrorCode getAlarmTimestamp(std::uint32_t *secs)
    {
        if (secs == nullptr) {
            return ErrorCode::Error;
        }

        return ErrorCode::OK;
    }

    ErrorCode enableAlarmIrq()
    {
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
