// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Result.hpp"
#include "re2/re2.h"

using namespace at;

Result::Result(std::vector<std::string> response,
               std::optional<StatusCode> statusCode,
               const Result::CmxError &errorCode)
    : errorCode(errorCode), statusCode(statusCode.value_or(StatusCode::NONE)), responseBody(std::move(response))
{}

auto Result::getEquipmentErrorCode() const -> std::optional<at::EquipmentErrorCode>
{
    if (std::holds_alternative<at::EquipmentErrorCode>(errorCode)) {
        return std::get<at::EquipmentErrorCode>(errorCode);
    }
    return std::nullopt;
}

auto Result::getNetworkErrorCode() const -> std::optional<at::NetworkErrorCode>
{
    if (std::holds_alternative<at::NetworkErrorCode>(errorCode)) {
        return std::get<at::NetworkErrorCode>(errorCode);
    }
    return std::nullopt;
}

auto Result::getResponse() const -> const std::vector<std::string> &
{
    return responseBody;
}

auto Result::getErrorCode() const -> const Result::CmxError &
{
    return errorCode;
}

auto Result::getStatusCode() const -> Result::StatusCode
{
    return statusCode;
}

void Result::setStatusCode(Result::StatusCode status)
{
    statusCode = status;
}
