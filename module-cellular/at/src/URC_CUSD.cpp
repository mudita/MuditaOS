// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URC_CUSD.hpp>

using namespace at::urc;

CUSD::CUSD(const std::string &val) : Any(val, '\"')
{}

auto CUSD::what() -> std::string
{
    return urc_name;
}

bool CUSD::isActionNeeded(void)
{
    constexpr uint32_t actionNeeded = 2;

    utils::findAndReplaceAll(tokens[static_cast<uint32_t>(CUSD::Tokens::Status)], ",", "");
    uint32_t status = 0;
    if (utils::toNumeric(tokens[static_cast<uint32_t>(CUSD::Tokens::Status)], status)) {
        if (status == actionNeeded) {
            return true;
        }
    }
    return false;
}

std::string CUSD::message(void)
{
    return tokens[static_cast<uint32_t>(CUSD::Tokens::Response)];
}
