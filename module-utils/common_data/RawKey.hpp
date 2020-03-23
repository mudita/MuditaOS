#pragma once

#include <cstdint>
#include <bsp/keyboard/key_codes.hpp>

/// default application timer trigger
const inline uint32_t key_timer_ms = 200;
/// default time key press will be counted as press again
const inline uint32_t key_time_cycle_ms = 500;
/// default long press time
const inline uint32_t key_time_longpress_ms = 1000;

struct RawKey
{
    enum class State
    {
        Undefined,
        Pressed,
        Released,
    } state                = State::Undefined;
    bsp::KeyCodes key_code = bsp::KeyCodes::Undefined;
    unsigned int time_press   = 0;
    unsigned int time_release = 0;

    /// set all values to zero, set new key_code and state
    void reset(bsp::KeyCodes key_code = bsp::KeyCodes::Undefined, State state = State::Undefined)
    {
        this->key_code = key_code;
        this->state    = state;
        time_press     = 0;
        time_release   = 0;
    }
};
