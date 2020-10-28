// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC_Any.hpp"

#include <optional>

namespace at::urc
{

    class CUSD : public Any
    {

        const std::string urc_name = "+CUSD";
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

        explicit CUSD(const std::string &val);
        ~CUSD() override = default;

        [[nodiscard]] auto what() const noexcept -> std::string final;
        [[nodiscard]] auto isValid() const noexcept -> bool;
        [[nodiscard]] auto isActionNeeded() const noexcept -> bool;
        [[nodiscard]] auto getMessage() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getStatus() const noexcept -> std::optional<StatusType>;
        [[nodiscard]] auto getDCS() const noexcept -> std::optional<int>;
    };
} // namespace at::urc
