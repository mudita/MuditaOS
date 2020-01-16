#pragma once
extern "C"
{
#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
}

inline const char STX = 02;
inline const char ETX = 02;
int hwInit(xQueueHandle qHandle);
