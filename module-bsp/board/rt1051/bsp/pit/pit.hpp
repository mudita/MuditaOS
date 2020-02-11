#pragma once

#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
#include <functional>

namespace bsp
{
    namespace pit
    {
        enum class Event
        {
            Overflow
        };
        void init(xQueueHandle qhandle);
        void start(uint32_t usec, std::function<void(void)> iqr_cb);
        void stop();
    }; // namespace pit
};     // namespace bsp
