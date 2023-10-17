// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UrcCmti.hpp"

using namespace at::urc;

auto Cmti::isValid() const noexcept -> bool
{
    return tokens.size() == magic_enum::enum_count<Tokens>();
}

std::string Cmti::getMemory() const
{
    if (!isValid()) {
        return std::string();
    }
    return tokens[Tokens::Mem];
}

std::string Cmti::getIndex() const
{
    if (!isValid()) {
        return std::string();
    }
    return tokens[Tokens::Index];
}
