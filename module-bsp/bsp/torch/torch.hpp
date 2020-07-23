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

        bool isPresent(void);
    } // namespace torch
} // namespace bsp
