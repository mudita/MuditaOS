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
        enum class Tray
        {
            OUT,
            IN
        } tray = Tray::IN;
        /// tray - tray actual status which is visible right now on screen
        /// selected - tray selection settings settable sim tray
        enum class SIM : uint8_t
        {
            SIM1,
            SIM2,
            NONE,
        } sim = SIM::NONE, selected = SIM::SIM1;
        static GSM *get();
    };
}; // namespace Store
