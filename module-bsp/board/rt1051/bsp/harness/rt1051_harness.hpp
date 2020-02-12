#pragma once
extern "C"
{
#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
}

#include "bsp/harness/bsp_pinmap.hpp"
#include <string>

inline const char STX = 0x02;
inline const char ETX = 0x03;
int hwInit(xQueueHandle qHandle);
std::string hwRead();
bool hwFlush();
bool hwEmit(const std::string &);
void hwGpioWrite(bsp::harness::gpio::Pin pin, uint32_t state);
bool hwGpioRead(bsp::harness::gpio::Pin pin);
