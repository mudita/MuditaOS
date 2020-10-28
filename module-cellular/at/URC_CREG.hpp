// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC_Any.hpp"

#include <common_data/EventStore.hpp>

namespace at::urc
{
    /// +CREG: <stat> - Indicate network registration status of the ME
    /// +CREG: <stat>[,<lac>,<ci>[,<Act>]] - Indicate network registration and location information of the ME
    class CREG : public Any
    {
        const std::string urc_name = "+CREG";
        enum Tokens
        {
            Stat,
            Lac,
            NumOfShortTokens = Lac,
            Ci,
            Act
        };

      public:
        explicit CREG(const std::string &val);
        ~CREG() override = default;

        [[nodiscard]] auto what() const noexcept -> std::string final;

        [[nodiscard]] auto isValid() const noexcept -> bool;
        [[nodiscard]] auto isExtended() const noexcept -> bool;
        [[nodiscard]] auto isShort() const noexcept -> bool;

        [[nodiscard]] auto getStatus() const noexcept -> Store::Network::Status;
        [[nodiscard]] auto getLocation() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getCellId() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getAccessTechnology() const noexcept -> Store::Network::AccessTechnology;
    };
}; // namespace at::urc
