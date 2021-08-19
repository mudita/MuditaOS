// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UrcQSimstat.hpp"

using namespace at::urc;

auto QSimstat::isValid() const noexcept -> bool
{
    return tokens.size() >= minParametersCount;
}
auto QSimstat::getInsertedStatus() const noexcept -> std::optional<at::SimInsertedStatus>
{
    auto status = 0;
    if (utils::toNumeric(tokens[magic_enum::enum_integer(Tokens::InsertedStatus)], status) &&
        magic_enum::enum_contains<SimInsertedStatus>(status)) {
        return static_cast<SimInsertedStatus>(status);
    }
    return std::nullopt;
}
auto QSimstat::getEnabled() const noexcept -> std::optional<at::SimInsertedStatusEnable>
{
    auto enabled = 0;
    if (utils::toNumeric(tokens[magic_enum::enum_integer(Tokens::Enable)], enabled) &&
        magic_enum::enum_contains<at::SimInsertedStatusEnable>(enabled)) {
        return static_cast<at::SimInsertedStatusEnable>(enabled);
    }
    return std::nullopt;
}
