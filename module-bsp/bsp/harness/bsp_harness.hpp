#pragma once

#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
#include <string>

#if defined(TARGET_RT1051)
#include "board/rt1051/bsp/harness/rt1051_harness.hpp"
#elif defined(TARGET_Linux)
#include "cellular/linux_cellular.hpp"
#else
#error "Unsupported target"
#endif

namespace bsp {
    namespace harness {
        int Init(xQueueHandle qHandle);
        std::string read();
        bool flush();
    };
};
