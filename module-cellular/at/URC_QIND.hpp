// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

namespace at::urc
{
    class QIND : public URC
    {

        const std::string type_csq = "csq";

        static const auto invalid_rssi_low  = 99;
        static const auto invalid_rssi_high = 199;
        static const auto invalid_ber       = 99;

        enum CSQ
        {
            CSQ,
            RSSI,
            BER
        };

        /// by docs invalid csq: RSSI: 99, 199, and ber: 99
        [[nodiscard]] auto validate(enum CSQ) const noexcept -> bool;

      public:
        inline static const std::string head = "+QIND";
        using URC::URC;

        ~QIND() override = default;

        [[nodiscard]] auto isCsq() const noexcept -> bool;
        [[nodiscard]] auto getRSSI() const noexcept -> std::optional<int>;
        [[nodiscard]] auto getBER() const noexcept -> std::optional<int>;
    };
}; // namespace at::urc
