#pragma once

#include "URC_Any.hpp"

namespace at::urc
{
    struct QIND : public Any
    {
        const std::string urc_name = "+QIND";
        const std::string type_csq = "csq";

        static const auto invalid_rssi_modulo = 99;
        static const auto invalid_ber = 99;

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

        QIND(const std::string &val);
        ~QIND() override = default;
        auto what() -> std::string final;
        enum CSQ
        {
            CSQ,
            RSSI,
            BER
        };

        auto is_csq() -> bool;
        /// by docs invalid csq: RSSI: 99, 199, and ber: 99
        auto validate(enum CSQ) -> bool;

        auto getBer() -> int
        {
            return std::stoi(tokens[BER]);
        }
        auto getRssi() -> int
        {
            return std::stoi(tokens[RSSI]);
        }
        auto getRssiDbm() -> int;

        /// @return 0 - if invalid
        static auto rssiTodBm(const int rssi) -> int;
    };
}; // namespace at::urc
