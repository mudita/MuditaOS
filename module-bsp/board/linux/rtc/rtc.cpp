// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/rtc/rtc.hpp>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <time.h>

namespace
{
    time_t timestampOffset;
    time_t timestampAlarm;
    time_t localOffset;
    xQueueHandle qHandleRtcIrq   = NULL;
    TaskHandle_t rtcWorkerHandle = NULL;

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

        xTaskCreate(rtcWorker, "rtc_worker", 512, qHandle, 0, &rtcWorkerHandle);
        return ErrorCode::OK;
    }

    ErrorCode setDateTimeFromTimestamp(time_t timestamp)
    {
        time_t current   = time(NULL);
        struct tm *local = localtime(&current);

        localOffset = local->tm_gmtoff;

        timestampOffset = timestamp - current;
        return ErrorCode::OK;
    }

    ErrorCode setDateTime(struct tm *tim)
    {

        time_t current   = time(NULL);
        time_t timestamp = mktime(tim);

        timestampOffset = timestamp - current;
        return ErrorCode::OK;
    }

    ErrorCode getCurrentDateTime(struct tm *datetime)
    {
        time_t t = time(NULL);
        t += timestampOffset;
        *datetime = *gmtime(&t);

        return ErrorCode::OK;
    }

    ErrorCode getCurrentTimestamp(time_t *timestamp)
    {
        *timestamp = time(NULL) + timestampOffset;

        return ErrorCode::OK;
    }

    ErrorCode setAlarmOnDate(struct tm *datetime)
    {
        if (datetime == NULL) {
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
        if (secs == NULL) {
            return ErrorCode::Error;
        }

        return ErrorCode::OK;
    }

    ErrorCode enableAlarmIrq()
    {
        std::uint32_t cnt = 100000;

        if (cnt == 0) {
            return ErrorCode::Error;
        }

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
