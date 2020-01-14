#pragma once

// static lifetime read only cache for (hw) states to not poll
// right now it serves data from:
// - battery
// it's not meant to serve as polling interface - rather to serve data

namespace Store
{
    struct Battery
    {
        enum class State
        {
            Discharging,
            Charging,
        } state = State::Discharging;
        unsigned int level = 0;

        static const Battery &get();
        static Battery &modify();
    };
}; // namespace Store
