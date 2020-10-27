// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URC_CUSD.hpp>
#include <Utils.hpp>
#include <magic_enum.hpp>

using namespace at::urc;

CUSD::CUSD(const std::string &val) : Any(val)
{}

auto CUSD::what() const noexcept -> std::string
{
    return urc_name;
}

auto CUSD::isValid() const noexcept -> bool
{
    return is() && tokens.size() == magic_enum::enum_count<Tokens>();
}

auto CUSD::isActionNeeded() const noexcept -> bool
{
    if (isValid()) {
        auto status = getStatus();
        if (status) {
            return *status == StatusType::FurtherUserActionRequired;
        }
    }
    return false;
}

auto CUSD::getMessage() const noexcept -> std::optional<std::string>
{
    if (!isValid()) {
        return std::nullopt;
    }

    auto message = tokens[Tokens::Response];
    utils::findAndReplaceAll(message, "\"", "");

    return utils::trim(message);
}

auto CUSD::getStatus() const noexcept -> std::optional<StatusType>
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

auto CUSD::getDCS() const noexcept -> std::optional<int>
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
