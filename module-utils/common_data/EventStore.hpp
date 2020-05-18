#pragma once

// static lifetime read only cache for (hw) states to not poll
// right now it serves data from:
// - battery
// - gsm SIM tray
// it's not meant to serve as polling interface - rather to serve data

#include <cstddef>

namespace cpp_freertos
{
    // fw decl
    class MutexStandard;
} // namespace cpp_freertos

namespace Store
{
    struct Battery
    {
        enum class State
        {
            Discharging,
            Charging,
        } state            = State::Discharging;
        unsigned int level = 0;

        static const Battery &get();
        static Battery &modify();
    };

    enum class RssiBar : size_t
    {
        zero  = 0,
        one   = 1,
        two   = 2,
        three = 3,
        four  = 4,
        five  = 5,
        noOfSupprtedBars
    };

    struct SignalStrength
    {
        int rssi        = 0;
        int rssidBm     = 0;
        RssiBar rssiBar = RssiBar::zero;
    };

    struct GSM
    {
      private:
        GSM() = default;
        SignalStrength signalStrength;

        static cpp_freertos::MutexStandard mutex;

      public:
        GSM(const GSM &) = delete;
        GSM &operator=(const GSM &) = delete;

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
        } sim      = SIM::SIM_UNKNOWN,
          selected = SIM::SIM1;

        /// state of modem
        enum class Modem
        {
            OFF,             /// modem is off - it's not working
            ON_INITIALIZING, /// modem is set to on, just started - initialization not done yet
            ON_NEED_SIMFLOW, /// modem is on, initialized, no SIM initialization yet
            ON_INITIALIZED,  /// modem is on, and it's fully initialized
        } modem = Modem::OFF;

        void setSignalStrength(const SignalStrength &signalStrength);
        SignalStrength getSignalStrength();

        static GSM *get();
    };
}; // namespace Store
