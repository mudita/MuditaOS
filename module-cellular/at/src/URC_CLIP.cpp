// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "URC_CLIP.hpp"

using namespace at::urc;

auto CLIP::isValid() const noexcept -> bool
{
    return tokens.size() >= Tokens::MinParametersCount;
}

std::string CLIP::getNumber() const
{
    if (!isValid()) {
        return std::string();
    }
    return tokens[Tokens::Number];
};

CLIP::AddressType CLIP::getType() const
{
    if (!isValid()) {
        return CLIP::AddressType::FailedToParse;
    }

    int addressType = magic_enum::enum_integer(CLIP::AddressType::FailedToParse);
    utils::toNumeric(tokens[Tokens::Type], addressType);

    constexpr auto addressTypes = magic_enum::enum_values<CLIP::AddressType>();
    for (const auto &type : addressTypes) {
        if (magic_enum::enum_integer(type) == addressType) {
            return type;
        }
    }

    return CLIP::AddressType::FailedToParse;
};

std::optional<std::string> CLIP::getSubaddr() const
{
    if (tokens.size() < Tokens::Subaddr + 1) {
        return std::nullopt;
    }
    if (tokens[Tokens::Subaddr].empty()) {
        return std::nullopt;
    }
    return tokens[Tokens::Subaddr];
};

std::optional<std::string> CLIP::getSatype() const
{
    if (tokens.size() < Tokens::Satype + 1) {
        return std::nullopt;
    }
    if (tokens[Tokens::Satype].empty()) {
        return std::nullopt;
    }
    return tokens[Tokens::Satype];
};

std::optional<std::string> CLIP::getAlpha() const
{
    if (tokens.size() < Tokens::Alpha + 1) {
        return std::nullopt;
    }
    if (tokens[Tokens::Alpha].empty()) {
        return std::nullopt;
    }
    return tokens[Tokens::Alpha];
};

std::optional<std::string> CLIP::getCLIValidity() const
{
    if (tokens.size() < Tokens::CLIValidity + 1) {
        return std::nullopt;
    }
    if (tokens[Tokens::CLIValidity].empty()) {
        return std::nullopt;
    }
    return tokens[Tokens::CLIValidity];
};
