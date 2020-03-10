#pragma once

// static lifetime read only cache for (hw) states to not poll
// right now it serves data from:
// - battery
// - gsm SIM tray
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
        enum class SIM
        {
            SIM1,
            SIM2,
            SIM_FAIL,
            SIM_UNKNOWN,
            NONE,
        } sim = SIM::SIM_UNKNOWN,
          selected = SIM::SIM1;

        /// state of modem
        enum class Modem
        {
            OFF,             /// modem is off - it's not working
            ON_INITIALIZING, /// modem is set to on, just started - initialization not done yet
            ON_NEED_SIMFLOW, /// modem is on, initialized, no SIM initialization yet
            ON_INITIALIZED,  /// modem is on, and it's fully initialized
        } modem = Modem::OFF;
        static GSM *get();
    };
}; // namespace Store
