// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryCalllogGetCount.hpp"

#include <string>

using namespace db::query;

CalllogGetCount::CalllogGetCount() : Query(Query::Type::Read)
{}

CalllogGetCountResult::CalllogGetCountResult(const uint32_t count) : count(count)
{}

[[nodiscard]] auto CalllogGetCount::debugInfo() const -> std::string
{
    return "CalllogGetCount";
}

[[nodiscard]] auto CalllogGetCountResult::debugInfo() const -> std::string
{
    return "CalllogGetCountResult";
}

auto CalllogGetCountResult::getCount() const -> uint32_t
{
    return count;
}
