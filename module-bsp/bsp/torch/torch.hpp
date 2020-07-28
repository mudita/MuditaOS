#pragma once

#include <stdint.h>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

#include "bsp/common.hpp"
#include <utility>

namespace bsp
{
    namespace torch
    {

        enum class Action
        {
            getState,
            setState,
            toggle,
        };

        enum class State
        {
            on,
            off,
        };

        int32_t init(xQueueHandle qHandle);
        void deinit();

        bool isPresent(void);

        bool turn(State state);
        std::pair<bool, State> getState();
        bool toggle();
        bool setCurrent(const unsigned short mA);
    } // namespace torch
} // namespace bsp
