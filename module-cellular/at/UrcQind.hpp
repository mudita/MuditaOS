// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"

#include <EventStore.hpp>

namespace at::urc
{
    class Qind : public Urc
    {

        const std::string type_csq           = "csq";
        const std::string type_fota          = "FOTA";
        const std::string type_act           = "act";
        const std::string_view type_sms_done = "SMS DONE";

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
            Fota,
            Stage,
            Param
        };

        enum ACT
        {
            ACT,
            ACTVALUE
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
        };

        static constexpr std::string_view head = "+QIND";
        static auto isURC(const std::string &uHead) -> bool
        {
            return uHead.find(Qind::head) != std::string::npos;
        }

        using Urc::Urc;

        [[nodiscard]] auto isCsq() const noexcept -> bool;
        [[nodiscard]] auto getRSSI() const noexcept -> std::optional<int>;
        [[nodiscard]] auto getBER() const noexcept -> std::optional<int>;

        [[nodiscard]] auto isFota() const noexcept -> bool;
        [[nodiscard]] auto isFotaValid() const noexcept -> bool;
        [[nodiscard]] auto getFotaStage() const noexcept -> std::optional<FotaStage>;
        [[nodiscard]] auto getFotaParameter() const noexcept -> std::string;

        [[nodiscard]] auto isSmsDone() const noexcept -> bool;

        [[nodiscard]] auto isAct() const noexcept -> bool;
        [[nodiscard]] auto getAct() const noexcept -> std::string;
        [[nodiscard]] auto getAccessTechnology() const noexcept -> Store::Network::AccessTechnology;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
