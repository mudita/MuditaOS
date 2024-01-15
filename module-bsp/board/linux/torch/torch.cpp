// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/torch/torch.hpp"
#include <log/log.hpp>

namespace bsp::torch
{
    namespace
    {
        State simulatedState                = State::Off;
        ColourTemperature currentColourTemp = ColourTemperature::Warmest;
    } // namespace

    std::int32_t init()
    {
        simulatedState = State::Off;
        return 1;
    }

    void deinit()
    {}

    bool isPresent()
    {
        return true;
    }

    bool turn(State state, ColourTemperature colourTemp)
    {
        simulatedState = state;
        if (colourTemp != ColourTemperature::NoChange) {
            currentColourTemp = colourTemp;
        }

        if (state == State::On) {
            LOG_INFO("Torch is ON \U0001f526 (%s)",
                     currentColourTemp == ColourTemperature::Warmest ? "warm \U0001f987" : "cold \U0001f535");
        }
        else {
            LOG_INFO("Torch is OFF");
        }
        return true;
    }

    State getState()
    {
        return simulatedState;
    }

    ColourTemperature getColorTemp()
    {
        return currentColourTemp;
    }

    bool toggle()
    {
        return turn(getState() == State::Off ? State::On : State::Off);
    };

    bool setCurrent(const unsigned short mA)
    {
        return true;
    }
} // namespace bsp::torch
