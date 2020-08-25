#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

class BtOnOff : public sys::DataMessage
{
  public:
    enum class State
    {
        On,
        Off
    } state;

    /// power on bt can be steered between 0x0 - 0xF
    enum class Power
    {
        Min = 1,
        Avg = 7,
        Max = 15,
    } power = Power::Max;

    /// pattern
    enum class Pattern
    {
        Something = 1,
    } pattern;

    BtOnOff(State state = State::Off, Power power = Power::Max, Pattern pattern = Pattern::Something)
        : sys::DataMessage(MessageType::BtMessage)
    {
        this->state   = state;
        this->power   = power;
        this->pattern = pattern;
    }
};
