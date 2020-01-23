#pragma once

// static lifetime read only cache for (hw) states to not poll
// right now it serves data from:
// - battery
// - gsm SIM tray
// it's not meant to serve as polling interface - rather to serve data

#include <bitset>

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

    struct GSM
    {
      private:
        static GSM *ptr;

      public:
        enum class Tray : uint8_t
        {
            SIM1,
            SIM2,
        };
        std::bitset<4> tray;
        static GSM *get();
    };
}; // namespace Store
