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

    class SignalStrength
    {
        static SignalStrength *ptr;
        SignalStrength() = default;

        // <rssi>   0           -113dBm or less
        //          1           -111dBm
        //          2...30      -109dBm...-53dBm
        //          31          -51dBm or greater
        //          99          Not known or not detectable
        //          100         -116dBm or less
        //          101         -115dBm
        //          102..190    -114dBm...-26dBm
        //          191         -25dBm or greater
        //          199         Not known or not detectable
        static const auto rssi_low = 0;
        static const auto rssi_max = 31;
        static const auto rssi_low_dBm = -113;
        static const auto rssi_max_dBm = -51;
        static const auto rssi_step = (rssi_low_dBm - rssi_max_dBm) / (rssi_max - rssi_low);

        static const auto rssi_tdscmda_low = 100;
        static const auto rssi_tdscmda_max = 191;
        static const auto rssi_tdscmda_low_dBm = -116;
        static const auto rssi_tdscmda_max_dBm = -25;
        static const auto rssi_tdscmda_step = (rssi_tdscmda_low_dBm - rssi_tdscmda_max_dBm) / (rssi_tdscmda_max - rssi_low);

        static const auto rssidBm_invalid = 0;
        static const auto rssidBm_five_bar_margin = -60;
        static const auto rssidBm_four_bar_margin = -75;
        static const auto rssidBm_three_bar_margin = -90;
        static const auto rssidBm_two_bar_margin = -100;
        static const auto rssidBm_one_bar_margin = -110;

      public:
        ~SignalStrength();

        enum class RssiBar
        {
            zero = 0x00,
            one = 0x01,
            two = 0x02,
            three = 0x03,
            four = 0x04,
            five = 0x05,
            invalid = 0xff
        };

        int rssi = 0;
        int rssidBm = 0;
        RssiBar rssiBar = RssiBar::invalid;

        void setRssi(int rssi);
        bool isValid()
        {
            return rssidBm != rssidBm_invalid;
        };

        static SignalStrength *get();

        /// @return 0 - if invalid
        int rssiTodBm(const int rssi);
        RssiBar rssidBmToBar(const int rssidBm);
    };
}; // namespace Store
