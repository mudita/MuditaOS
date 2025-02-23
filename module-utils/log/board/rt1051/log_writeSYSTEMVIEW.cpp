// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <log/log.hpp>
#include "../../segger/systemview/SEGGER/SEGGER_SYSVIEW.h"

void log_WriteToDevice(const uint8_t *pBuffer, unsigned NumBytes)
{
    SEGGER_SYSVIEW_PrintfHost((const char *)pBuffer);
}
