#pragma once
extern "C"
{
#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
}

#include <string>
#include "bsp/harness/bsp_pinmap.hpp"

inline const char STX = 0x02;
inline const char ETX = 0x03;
int hwInit(xQueueHandle qHandle);
std::string hwRead();
bool hwFlush();
bool hwEmit(const std::string &);
bool hwGpioRead(enum bsp::harness::gpio::Pin el);
void hwGpioWrite(enum bsp::harness::gpio::Pin el, uint32_t state);
