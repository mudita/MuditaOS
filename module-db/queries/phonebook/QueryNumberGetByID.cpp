// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNumberGetByID.hpp"

#include <Common/Query.hpp>
#include <PhoneNumber.hpp>

#include <string>

using namespace db::query;

NumberGetByID::NumberGetByID(std::uint32_t id) : Query(Query::Type::Read), id(id)
{}

NumberGetByIDResult::NumberGetByIDResult(utils::PhoneNumber::View number) : number(std::move(number))
{}

[[nodiscard]] auto NumberGetByID::debugInfo() const -> std::string
{
    return "NumberGetByID";
}

[[nodiscard]] auto NumberGetByIDResult::debugInfo() const -> std::string
{
    return "NumberGetByIDResult";
}

auto NumberGetByID::getID() const noexcept -> std::uint32_t
{
    return id;
}

auto NumberGetByIDResult::getNumber() -> utils::PhoneNumber::View
{
    return std::move(number);
}
