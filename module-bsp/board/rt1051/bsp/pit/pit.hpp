#pragma once

#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>

namespace bsp
{
    namespace pit
    {
        enum class Event
        {
            Overflow
        };
        void init(xQueueHandle qhandle);
        void start(uint32_t usec);
        void stop();
    }; // namespace pit
};     // namespace bsp
