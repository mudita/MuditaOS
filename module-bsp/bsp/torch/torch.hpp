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

        enum ColourTemperature // Kelvin
        {
            no_change = 0,
            warmest = 1800,
            coldest = 6500,
        };

        int32_t init(xQueueHandle qHandle);
        void deinit();

        bool isPresent(void);

        bool turn(State state, ColourTemperature = no_change);
        std::pair<bool, State> getState();
        ColourTemperature getColorTemp(); // rather only for cert
        bool toggle();
        bool setCurrent(const unsigned short mA);
    } // namespace torch
} // namespace bsp
