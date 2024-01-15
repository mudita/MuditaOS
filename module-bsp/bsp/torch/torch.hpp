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
        On,
        Off
    };

    enum class ColourTemperature // Kelvin
    {
        NoChange = 0,
        Warmest = 1800,
        Coldest = 6500
    };

    std::int32_t init();
    void deinit();

    bool isPresent();

    bool turn(State state, ColourTemperature = ColourTemperature::NoChange);
    State getState();
    ColourTemperature getColorTemp();
    bool toggle();
    bool setCurrent(const unsigned short mA);
} // namespace bsp::torch
