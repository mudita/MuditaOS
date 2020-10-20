// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <FreeRTOS.h>
#include <queue.h>

class BluetoothStreamData
{
  public:
    QueueHandle_t in    = nullptr;
    QueueHandle_t out   = nullptr;
};
