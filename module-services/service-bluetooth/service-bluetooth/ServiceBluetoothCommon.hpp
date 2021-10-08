// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Bluetooth/Device.hpp>

#include <FreeRTOS.h>
#include <queue.h>

class BluetoothStreamData
{
  public:
    QueueHandle_t in  = nullptr;
    QueueHandle_t out = nullptr;
    DeviceMetadata_t metadata;
    BluetoothStreamData(QueueHandle_t in, QueueHandle_t out, DeviceMetadata_t metadata)
        : in(in), out(out), metadata(metadata)
    {}
};
