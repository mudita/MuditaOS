// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

#include <optional>

namespace at::urc
{

    class CUSD : public URC
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

        inline static const std::string head = "+CUSD";

        using URC::URC;
        ~CUSD() override = default;

        [[nodiscard]] bool isValid() const noexcept override;

        [[nodiscard]] auto isActionNeeded() const noexcept -> bool;
        [[nodiscard]] auto getMessage() const noexcept -> std::optional<std::string>;
        [[nodiscard]] auto getStatus() const noexcept -> std::optional<StatusType>;
        [[nodiscard]] auto getDCS() const noexcept -> std::optional<int>;
    };
} // namespace at::urc
