// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utility>
#include <variant>
#include <string>
#include <vector>
#include <optional>

#include <ErrorCode.hpp>

namespace at
{
    class Result
    {
      public:
        using CmxError = std::variant<at::EquipmentErrorCode, at::NetworkErrorCode>;

        enum class StatusCode
        {
            OK,      // at OK
            ERROR,   // at ERROR
            TIMEOUT, // at Timeout
            TOKENS,  // at numbers of tokens needed met
            NONE,
        };

        Result(std::vector<std::string> response    = std::vector<std::string>(),
               std::optional<StatusCode> statusCode = StatusCode::NONE,
               const CmxError &errorCode            = at::EquipmentErrorCode::NoInformation);

        virtual ~Result() = default;

        auto getResponse() const -> const std::vector<std::string> &;

        auto getErrorCode() const -> const CmxError &;
        auto getEquipmentErrorCode() const -> std::optional<at::EquipmentErrorCode>;
        auto getNetworkErrorCode() const -> std::optional<at::NetworkErrorCode>;

        auto getStatusCode() const -> StatusCode;
        void setStatusCode(StatusCode status);

        explicit operator bool() const
        {
            return statusCode == StatusCode::OK;
        }

      private:
        CmxError errorCode    = at::EquipmentErrorCode::NoInformation;
        StatusCode statusCode = StatusCode::NONE;
        const std::vector<std::string> responseBody;
    };
} // namespace at
