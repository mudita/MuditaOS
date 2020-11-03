// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

#include <common_data/EventStore.hpp>

namespace at::urc
{
    /// +CREG: <stat> - Indicate network registration status of the ME
    /// +CREG: <stat>[,<lac>,<ci>[,<Act>]] - Indicate network registration and location information of the ME
    class CREG : public URC
    {
        enum Tokens
        {
            Stat,
            Lac,
            NumOfShortTokens = Lac,
            Ci,
            Act
        };

      public:
        inline static const std::string head = "+CREG";
        static bool isURC(const std::string uHead)
        {
            return uHead.find(CREG::head) != std::string::npos;
        }

        using URC::URC;

        [[nodiscard]] bool isValid() const noexcept override;
        [[nodiscard]] auto isExtended() const noexcept -> bool;
        [[nodiscard]] auto isShort() const noexcept -> bool;

        [[nodiscard]] auto getStatus() const noexcept -> Store::Network::Status;
        [[nodiscard]] auto getLocation() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getCellId() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getAccessTechnology() const noexcept -> Store::Network::AccessTechnology;

        void Handle(URCHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
