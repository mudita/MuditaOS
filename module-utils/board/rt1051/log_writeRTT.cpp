// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>
#include "../../segger/rtt/SEGGER_RTT.h"

#define LOG_RTT_BUFF_INDEX 0

void log_WriteToDevice(const uint8_t *pBuffer, unsigned NumBytes)
{
    SEGGER_RTT_Write(LOG_RTT_BUFF_INDEX, pBuffer, NumBytes);
}
