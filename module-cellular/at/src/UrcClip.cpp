// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UrcClip.hpp"

using namespace at::urc;

auto Clip::isValid() const noexcept -> bool
{
    return tokens.size() >= minParametersCount;
}

std::string Clip::getNumber() const
{
    if (!isValid()) {
        return std::string();
    }
    return tokens[magic_enum::enum_integer(Tokens::Number)];
};

std::optional<Clip::AddressType> Clip::getType() const
{
    if (!isValid()) {
        return std::nullopt;
    }

    auto addressType = utils::getNumericValue<int>(tokens[magic_enum::enum_integer(Tokens::Type)]);

    constexpr auto addressTypes = magic_enum::enum_values<Clip::AddressType>();
    for (const auto &type : addressTypes) {
        if (magic_enum::enum_integer(type) == addressType) {
            return type;
        }
    }

    return std::nullopt;
};

std::optional<std::string> Clip::getSubaddr() const
{
    size_t minNumOfParams = magic_enum::enum_integer(Tokens::Subaddr) + 1;
    if (tokens.size() < minNumOfParams) {
        return std::nullopt;
    }
    if (tokens[magic_enum::enum_integer(Tokens::Subaddr)].empty()) {
        return std::nullopt;
    }
    return tokens[magic_enum::enum_integer(Tokens::Subaddr)];
};

std::optional<std::string> Clip::getSatype() const
{
    size_t minNumOfParams = magic_enum::enum_integer(Tokens::Satype) + 1;
    if (tokens.size() < minNumOfParams) {
        return std::nullopt;
    }
    if (tokens[magic_enum::enum_integer(Tokens::Satype)].empty()) {
        return std::nullopt;
    }
    return tokens[magic_enum::enum_integer(Tokens::Satype)];
};

std::optional<std::string> Clip::getAlpha() const
{
    size_t minNumOfParams = magic_enum::enum_integer(Tokens::Alpha) + 1;
    if (tokens.size() < minNumOfParams) {
        return std::nullopt;
    }
    if (tokens[magic_enum::enum_integer(Tokens::Alpha)].empty()) {
        return std::nullopt;
    }
    return tokens[magic_enum::enum_integer(Tokens::Alpha)];
};

std::optional<std::string> Clip::getCLIValidity() const
{
    size_t minNumOfParams = magic_enum::enum_integer(Tokens::CLIValidity) + 1;
    if (tokens.size() < minNumOfParams) {
        return std::nullopt;
    }
    if (tokens[magic_enum::enum_integer(Tokens::CLIValidity)].empty()) {
        return std::nullopt;
    }
    return tokens[magic_enum::enum_integer(Tokens::CLIValidity)];
};
