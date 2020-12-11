// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryEventsGetAllLimited.hpp"

namespace db::query::events
{
    GetAllLimited::GetAllLimited(const uint32_t &offset, const uint32_t &limit)
        : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto GetAllLimited::debugInfo() const -> std::string
    {
        return "GetAllLimited";
    }

    GetAllLimitedResult::GetAllLimitedResult(std::vector<EventsRecord> records, uint32_t count)
        : records{std::move(records)}, recordsCount{count}
    {}

    auto GetAllLimitedResult::getResult() -> std::vector<EventsRecord>
    {
        return records;
    }

    auto GetAllLimitedResult::getCountResult() const noexcept -> uint32_t
    {
        return recordsCount;
    }

    auto GetAllLimitedResult::debugInfo() const -> std::string
    {
        return "GetAllLimitedResult";
    }
} // namespace db::query::events
