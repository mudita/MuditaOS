// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>
#include "Urc.hpp"

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

        bool valid_ = true;

        auto split(const std::string &str) -> void override;
        [[nodiscard]] auto getDCS() const noexcept -> std::optional<int>;

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

        static constexpr std::string_view head = "+CUSD";

        explicit Cusd(const std::string &urcBody, const std::string &urcHead = {});

        static auto isURC(const std::string &uHead) -> bool;
        [[nodiscard]] auto isValid() const noexcept -> bool override;
        [[nodiscard]] auto isActionNeeded() const noexcept -> bool;
        [[nodiscard]] auto getMessage() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getStatus() const noexcept -> StatusType;

        auto Handle(UrcHandler &h) -> void final;
    };
} // namespace at::urc
