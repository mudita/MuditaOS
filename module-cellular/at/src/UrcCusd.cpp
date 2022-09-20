// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <UrcCusd.hpp>
#include <Utils.hpp>
#include <magic_enum.hpp>

using namespace at::urc;

Cusd::Cusd(const std::string &urcBody, const std::string &urcHead) : Urc(urcBody, urcHead)
{
    split(urcBody);
}
auto Cusd::isValid() const noexcept -> bool
{
    return tokens.size() == magic_enum::enum_count<Tokens>();
}

auto Cusd::isActionNeeded() const noexcept -> bool
{
    if (isValid()) {
        auto status = getStatus();
        if (status) {
            return *status == StatusType::FurtherUserActionRequired;
        }
    }
    return false;
}

auto Cusd::getMessage() const noexcept -> std::optional<std::string>
{
    if (!isValid()) {
        return std::nullopt;
    }

    auto message = tokens[Tokens::Response];
    utils::findAndReplaceAll(message, "\"", "");

    return utils::trim(message);
}

auto Cusd::getStatus() const noexcept -> std::optional<StatusType>
{
    if (isValid()) {
        int statusInt;
        try {
            statusInt = std::stoi(tokens[Tokens::Status]);
        }
        catch (const std::exception &e) {
            return std::nullopt;
        }

        auto status = magic_enum::enum_cast<StatusType>(statusInt);
        if (status.has_value()) {
            return status.value();
        }
    }

    return std::nullopt;
}

auto Cusd::getDCS() const noexcept -> std::optional<int>
{
    if (!isValid()) {
        return std::nullopt;
    }
    int dcs;
    try {
        dcs = std::stoi(tokens[Tokens::DCS]);
    }
    catch (const std::exception &e) {
        return std::nullopt;
    }

    return dcs;
}
void Cusd::split(const std::string &str)
{
    constexpr auto commaString    = ",";
    constexpr char tokenDelimiter = '\"';

    tokens = utils::split(str, tokenDelimiter);
    for (auto &t : tokens) {
        utils::findAndReplaceAll(t, "\"", "");
        t = utils::trim(t);
    }

    if (not isValid()) {
        return;
    }

    auto dcs       = tokens[Tokens::DCS];
    auto dcsTokens = utils::split(dcs, commaString);

    if (dcsTokens.size() > 1) {
        tokens.pop_back();
        for (auto el : dcsTokens) {
            tokens.push_back(el);
        }
    }
    uint32_t token = 0;
    for (auto &t : tokens) {
        if (token != static_cast<uint32_t>(Tokens::Response)) {
            utils::findAndReplaceAll(t, commaString, "");
            t = utils::trim(t);
        }
        token += 1;
    }
}
