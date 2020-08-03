#pragma once

#include "FreeRTOS.h"
#include "queue.h"


namespace bsp {
 namespace headset {
 int32_t Init(xQueueHandle qHandle);
 	void Handler(uint8_t notification);
 	 bool IsInserted(void);
 	int32_t Deinit(void);

 }
}
