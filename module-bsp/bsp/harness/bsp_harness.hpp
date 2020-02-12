#pragma once

#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
#include <string>
#include "bsp_pinmap.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/bsp/harness/rt1051_harness.hpp"
#elif defined(TARGET_Linux)
#include "harness/harness.hpp"
#else
#error "Unsupported target"
#endif

namespace bsp {
    namespace harness {
        int Init(xQueueHandle qHandle);
        std::string read();
        bool flush();
        bool emit(const std::string evt);
        namespace gpio
        {
            /// int to Pin number for harness
            Pin to(int which);
            bool read(enum Pin num);
            void write(enum Pin num, uint32_t state);
        }; // namespace gpio
    };
};
