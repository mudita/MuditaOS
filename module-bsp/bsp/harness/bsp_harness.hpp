#pragma once

#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>

namespace bsp {
    namespace harness {
        int Init(xQueueHandle qHandle);
    };
};
