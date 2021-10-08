// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"

#include <optional>

namespace at::urc
{

    class Cusd : public Urc
    {

        enum Tokens
        {
            Status,
            Response,
            DCS
        };

      public:
        enum class StatusType
        {
            NoFurtherUserActionRequired,
            FurtherUserActionRequired,
            UssdTerminatedByNetwork,
            AnotherLocalClientHasResponded,
            OperationNotSupported,
            NetworkTimeOut
        };
        Cusd(const std::string &urcBody, const std::string &urcHead = std::string());
        static constexpr std::string_view head = "+CUSD";
        static auto isURC(const std::string &uHead) -> bool
        {
            return uHead.find(Cusd::head) != std::string::npos;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;

        [[nodiscard]] auto isActionNeeded() const noexcept -> bool;
        [[nodiscard]] auto getMessage() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getStatus() const noexcept -> std::optional<StatusType>;
        [[nodiscard]] auto getDCS() const noexcept -> std::optional<int>;
        void split(const std::string &str) override;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
