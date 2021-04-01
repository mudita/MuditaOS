#pragma once

#include "FreeRTOS.h"
#include "queue.h"


namespace bsp {
 namespace headset {

    int32_t Init(xQueueHandle qHandle);

    bool Handler(uint8_t notification);

    bool IsInserted();

    int32_t Deinit();

    BaseType_t headset_IRQHandler();
 }
}
