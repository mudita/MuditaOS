// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "URC_CMTI.hpp"

using namespace at::urc;

auto CMTI::isValid() const noexcept -> bool
{
    return tokens.size() == magic_enum::enum_count<Tokens>();
}

std::string CMTI::getMemory() const
{
    if (!isValid()) {
        return std::string();
    }
    return tokens[Tokens::Mem];
}

std::string CMTI::getIndex() const
{
    if (!isValid()) {
        return std::string();
    }
    return tokens[Tokens::Index];
}
