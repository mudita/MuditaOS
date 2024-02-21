// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <hal/key_input/KeyEventDefinitions.hpp>

/// default application timer trigger
const inline uint32_t keyTimerMs = 200;
/// default time key press will be counted as press again
const inline uint32_t keyTimeCycleMs = 1200;
/// default long press time
const inline uint32_t keyTimeLongpressMs = 1000;

struct RawKey
{
    enum class State
    {
        Undefined, /// Undefined key state
        Pressed,   /// Bistable key pressed
        Released,  /// Bistable key released
        Moved,     /// Monostable key moved
    } state                  = State::Undefined;
    bsp::KeyCodes keyCode    = bsp::KeyCodes::Undefined;
    unsigned int timePress   = 0;
    unsigned int timeRelease = 0;

    /// set all values to zero, set new keyCode and state
    void reset(bsp::KeyCodes keyCode = bsp::KeyCodes::Undefined, State state = State::Undefined)
    {
        this->keyCode = keyCode;
        this->state   = state;
        timePress     = 0;
        timeRelease   = 0;
    }
};
