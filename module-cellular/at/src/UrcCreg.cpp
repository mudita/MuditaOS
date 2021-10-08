// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <UrcCreg.hpp>
#include <magic_enum.hpp>

using namespace at::urc;

auto Creg::isValid() const noexcept -> bool
{
    return isExtended() || isShort();
}

auto Creg::isExtended() const noexcept -> bool
{
    return tokens.size() == magic_enum::enum_count<Tokens>();
}

auto Creg::isShort() const noexcept -> bool
{
    return tokens.size() == NumOfShortTokens;
}

auto Creg::getStatus() const noexcept -> Store::Network::Status
{
    if (isValid()) {
        int statusInt;
        try {
            statusInt = std::stoi(tokens[Tokens::Stat]);
        }
        catch (const std::exception &e) {
            return Store::Network::Status::Unknown;
        }

        auto status = magic_enum::enum_cast<Store::Network::Status>(statusInt);
        if (status.has_value()) {
            return status.value();
        }
    }
    return Store::Network::Status::Unknown;
}

auto Creg::getLocation() const noexcept -> std::optional<std::string>
{
    if (isValid() && isExtended()) {
        auto location = tokens[Tokens::Lac];
        utils::findAndReplaceAll(location, "\"", "");
        return location;
    }

    return std::nullopt;
}

auto Creg::getCellId() const noexcept -> std::optional<std::string>
{
    if (isValid() && isExtended()) {
        auto cellId = tokens[Tokens::Ci];
        utils::findAndReplaceAll(cellId, "\"", "");
        return cellId;
    }

    return std::nullopt;
}

auto Creg::getAccessTechnology() const noexcept -> Store::Network::AccessTechnology
{
    if (isValid() && isExtended()) {
        int accessTechnologyInt;
        try {
            accessTechnologyInt = std::stoi(tokens[Tokens::Act]);
        }
        catch (const std::exception &e) {
            return Store::Network::AccessTechnology::Unknown;
        }

        auto accessTechnology = magic_enum::enum_cast<Store::Network::AccessTechnology>(accessTechnologyInt);
        if (accessTechnology.has_value()) {
            return accessTechnology.value();
        }
    }
    return Store::Network::AccessTechnology::Unknown;
}
