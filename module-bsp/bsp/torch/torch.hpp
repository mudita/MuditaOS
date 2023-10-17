#pragma once

#include <stdint.h>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

#include "bsp/common.hpp"

namespace bsp::torch
{
    enum class State
    {
        on,
        off,
    };

    enum class ColourTemperature // Kelvin
    {
        noChange = 0,
        warmest = 1800,
        coldest = 6500,
    };

    std::int32_t init();
    void deinit();

    bool isPresent(void);

    bool turn(State state, ColourTemperature = ColourTemperature::noChange);
    State getState();
    ColourTemperature getColorTemp();
    bool toggle();
    bool setCurrent(const unsigned short mA);
} // namespace bsp::torch
