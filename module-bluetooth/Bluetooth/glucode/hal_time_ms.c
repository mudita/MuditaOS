#include "btstack_debug.h"
#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>

uint32_t hal_time_ms(void)
{
    return xTaskGetTickCount();
}
