#pragma once

#include "URC_Any.hpp"

namespace at::urc
{
    struct QIND : public Any
    {
        static const auto invalid_rssi = 99;
        static const auto invalid_ber = 99;

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
    };
}; // namespace at::urc
