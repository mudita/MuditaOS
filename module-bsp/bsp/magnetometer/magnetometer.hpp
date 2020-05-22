#pragma once

#include <stdint.h>

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
}

#include "../common.hpp"

namespace bsp {

namespace magnetometer{

	int32_t init(xQueueHandle qHandle);

	bool isPresent(void);
	bsp::Board GetBoard(void);
}

}
