#pragma once

#include <stdint.h>

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
}

namespace bsp {


namespace magnetometer{

	int32_t init(xQueueHandle qHandle);

	bool isPresent(void);
}

}
