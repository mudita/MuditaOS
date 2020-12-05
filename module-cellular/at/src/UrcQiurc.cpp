// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UrcQiurc.hpp"

using namespace at::urc;

auto Qiurc::isValid() const noexcept -> bool
{
    return tokens.size() == qiurcPdpdeactCount; /// only support one message type
}

auto Qiurc::getType() const noexcept -> std::optional<QIUrcMessages>
{
    if (!isValid()) {
        return std::nullopt;
    }

    if (tokens[Tokens::Type] == qiurcPdpdeact) {
        return QIUrcMessages::DeactivateContext;
    }
    return std::nullopt;
}

auto Qiurc::getFirstParam() const noexcept -> std::optional<std::string>
{
    if (getType()) {
        if (isValid() && (*getType() == QIUrcMessages::DeactivateContext)) {
            return tokens[Tokens::FirstParam];
        }
    }
    return std::nullopt;
}
