// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"

#include <EventStore.hpp>

namespace at::urc
{
    /// +CREG: <stat> - Indicate network registration status of the ME
    /// +CREG: <stat>[,<lac>,<ci>[,<Act>]] - Indicate network registration and location information of the ME
    class Creg : public Urc
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
        static constexpr std::string_view head = "+CREG";
        static bool isURC(const std::string &uHead)
        {
            return uHead.find(Creg::head) != std::string::npos;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;
        [[nodiscard]] auto isExtended() const noexcept -> bool;
        [[nodiscard]] auto isShort() const noexcept -> bool;

        [[nodiscard]] auto getStatus() const noexcept -> Store::Network::Status;
        [[nodiscard]] auto getLocation() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getCellId() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getAccessTechnology() const noexcept -> Store::Network::AccessTechnology;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
