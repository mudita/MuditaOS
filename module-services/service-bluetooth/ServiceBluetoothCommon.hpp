#pragma once

#include <FreeRTOS.h>
#include <queue.h>

class BluetoothStreamData
{
  public:
    QueueHandle_t in  = nullptr;
    QueueHandle_t out = nullptr;
};
