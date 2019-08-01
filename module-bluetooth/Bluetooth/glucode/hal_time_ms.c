#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>

uint32_t hal_time_ms(void)
{
    return  xTaskGetTickCount();
}
