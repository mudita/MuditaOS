// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryCalllogGetCount.hpp"

#include <string>

using namespace db::query;

CalllogGetCount::CalllogGetCount(EntryState state) : Query(Query::Type::Read), state(state)
{}

auto CalllogGetCount::getState() const noexcept -> EntryState
{
    return state;
}

CalllogGetCountResult::CalllogGetCountResult(EntryState state, unsigned count) : state(state), count(count)
{}

auto CalllogGetCountResult::getState() const noexcept -> EntryState
{
    return state;
}

[[nodiscard]] auto CalllogGetCount::debugInfo() const -> std::string
{
    return "CalllogGetCount";
}

[[nodiscard]] auto CalllogGetCountResult::debugInfo() const -> std::string
{
    return "CalllogGetCountResult";
}

auto CalllogGetCountResult::getCount() const noexcept -> unsigned
{
    return count;
}
