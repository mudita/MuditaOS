#pragma once

#include <stdint.h>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

#include "bsp/common.hpp"

namespace bsp
{
    namespace torch
    {

        int32_t init(xQueueHandle qHandle);
        void deinit();

        bool isPresent(void);

        bool turn(bool state);
        bool isOn();
        bool setCurrent(const unsigned short mA);
    } // namespace torch
} // namespace bsp
