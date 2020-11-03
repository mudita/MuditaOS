// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

namespace at::urc
{
    class QIND : public URC
    {

        const std::string type_csq = "csq";
        const std::string type_fota = "FOTA";

        static const auto invalid_rssi_low  = 99;
        static const auto invalid_rssi_high = 199;
        static const auto invalid_ber       = 99;

        enum CSQ
        {
            CSQ,
            RSSI,
            BER
        };

        enum FOTA
        {
            FOTA,
            STAGE,
            PARAM
        };

        const size_t fotaMinTokenSize = 2;

        /// by docs invalid csq: RSSI: 99, 199, and ber: 99
        [[nodiscard]] auto validate(enum CSQ) const noexcept -> bool;

      public:
        enum class FotaStage
        {
            HTTPSTART,
            HTTPEND,
            START,
            UPDATING,
            END,
            UNDEFINED
        };

        inline static const std::string head = "+QIND";
        static bool isURC(const std::string uHead)
        {
            return uHead.find(QIND::head) != std::string::npos;
        }

        using URC::URC;

        [[nodiscard]] auto isCsq() const noexcept -> bool;
        [[nodiscard]] auto getRSSI() const noexcept -> std::optional<int>;
        [[nodiscard]] auto getBER() const noexcept -> std::optional<int>;

        [[nodiscard]] auto isFota() const noexcept -> bool;
        [[nodiscard]] auto isFotaValid() const noexcept -> bool;
        [[nodiscard]] auto getFotaStage() const noexcept -> FotaStage;
        [[nodiscard]] auto getFotaParameter() const noexcept -> std::string;

        void Handle(URCHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
