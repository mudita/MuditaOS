// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ctime>

extern "C"
{
    time_t FreeRTOS_time(time_t *pxTime)
    {
        return std::time(pxTime);
    }
}
