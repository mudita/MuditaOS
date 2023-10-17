// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryThreadsGetCount.hpp"

db::query::ThreadGetCount::ThreadGetCount(EntryState state) : Query(Query::Type::Read), state(state)
{}

auto db::query::ThreadGetCount::debugInfo() const -> std::string
{
    return "SMSThreadGetCount";
}

auto db::query::ThreadGetCount::getState() const noexcept -> EntryState
{
    return state;
}

db::query::ThreadGetCountResult::ThreadGetCountResult(EntryState state, unsigned count) : state(state), count(count)
{}

auto db::query::ThreadGetCountResult::getCount() const noexcept -> unsigned
{
    return count;
}
auto db::query::ThreadGetCountResult::getState() const noexcept -> EntryState
{
    return state;
}

[[nodiscard]] auto db::query::ThreadGetCountResult::debugInfo() const -> std::string
{
    return "SMSThreadGetCountResult";
}
