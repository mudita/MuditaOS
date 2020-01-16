#pragma once
extern "C"
{
#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
}

#include <string>

inline const char STX = 02;
inline const char ETX = 03;
int hwInit(xQueueHandle qHandle);
std::string hwRead();
bool hwFlush();
