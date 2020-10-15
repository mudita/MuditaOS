#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#include <optional>

namespace bsp
{
    namespace vibrator
    {
        enum class State
        {
            on,
            off,
        };

        class Vibration
        {
          public:
            /// default (null) duration means forever
            State state;
            std::optional<uint16_t> duration;
            Vibration(State state, std::optional<uint16_t> duration = std::nullopt)
                : state(state), duration(duration){};
        };

        void init();
        void deinit();
        void enable();
        void disable();
    } // namespace vibrator
} // namespace bsp
