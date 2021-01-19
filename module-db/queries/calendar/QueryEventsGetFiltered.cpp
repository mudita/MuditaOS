// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryEventsGetFiltered.hpp"

namespace db::query::events
{
    GetFiltered::GetFiltered(calendar::TimePoint filter_from,
                             calendar::TimePoint filter_till,
                             uint32_t offset,
                             uint32_t limit)
        : Query(Query::Type::Read), filter_from(filter_from), filter_till(filter_till), offset(offset), limit(limit)
    {}

    auto GetFiltered::debugInfo() const -> std::string
    {
        return "GetFiltered";
    }

    GetFilteredResult::GetFilteredResult(std::vector<EventsRecord> records, uint32_t count)
        : records{std::move(records)}, recordsCount{count}
    {}

    auto GetFilteredResult::getResult() -> std::vector<EventsRecord>
    {
        return records;
    }

    auto GetFilteredResult::getCountResult() -> uint32_t
    {
        return recordsCount;
    }

    auto GetFilteredResult::debugInfo() const -> std::string
    {
        return "GetFilteredResult";
    }
} // namespace db::query::events
