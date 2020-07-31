#pragma once

#include "FreeRTOS.h"
#include "queue.h"


namespace bsp {
 namespace headset {
 int32_t Init(xQueueHandle qHandle);
 	 BaseType_t IRQHandler(void);
 	 void Handler(void);
 	 bool IsInserted(void);
 	int32_t Deinit(void);

 }
}
