// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNumbersGetByIDs.hpp"

#include <Common/Query.hpp>
#include <PhoneNumber.hpp>

#include <string>

using namespace db::query;

NumbersGetByIDs::NumbersGetByIDs(const std::vector<std::uint32_t> &numberIDs)
    : Query(Query::Type::Read), numberIDs(numberIDs)
{}

NumbersGetByIDsResult::NumbersGetByIDsResult(const std::map<std::uint32_t, utils::PhoneNumber::View> &numbers)
    : numbers((numbers))
{}

[[nodiscard]] auto NumbersGetByIDs::debugInfo() const -> std::string
{
    return "NumbersGetByIDs";
}

[[nodiscard]] auto NumbersGetByIDsResult::debugInfo() const -> std::string
{
    return "NumberGetByIDResult";
}

auto NumbersGetByIDs::getIDs() const noexcept -> std::vector<std::uint32_t>
{
    return numberIDs;
}

auto NumbersGetByIDsResult::getNumbers() -> std::map<std::uint32_t, utils::PhoneNumber::View>
{
    return std::move(numbers);
}
