#pragma once
extern "C"
{
#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
}

#include <string>

inline const char STX = 0x02;
inline const char ETX = 0x03;
int hwInit(xQueueHandle qHandle);
std::string hwRead();
bool hwFlush();
