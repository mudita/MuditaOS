// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <FreeRTOS.h>
#include <queue.h>
#include <ctime>
#include <cstdint>

namespace bsp::rtc {
	enum class IrqNotification
    {
        AlarmOccurred = 0x01
    };

	enum class ErrorCode
	{
		OK,
		Error
	};

	// RTC functions are using FreeRTOS functions, RTC should be initialized after scheduler starts.

	ErrorCode init(xQueueHandle qHandle);
	ErrorCode enableAlarmIrq();
	ErrorCode disableAlarmIrq();
	ErrorCode maskAlarmIrq();
	ErrorCode unmaskAlarmIrq();
	ErrorCode setDateTimeFromTimestamp(time_t timestamp);
	ErrorCode setDateTime(struct tm* time);
	ErrorCode getCurrentDateTime(struct tm* datetime);
	ErrorCode getCurrentTimestamp(time_t* timestamp);
	ErrorCode setAlarmOnDate(struct tm* datetime);
	ErrorCode setAlarmOnTimestamp(std::uint32_t secs);
	ErrorCode setAlarmInSecondsFromNow(std::uint32_t secs);
	ErrorCode getAlarmTimestamp(std::uint32_t* secs);
	ErrorCode setMinuteAlarm(time_t timestamp);
}
