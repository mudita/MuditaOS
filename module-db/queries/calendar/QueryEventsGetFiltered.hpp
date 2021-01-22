// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>
#include <module-apps/application-calendar/data/dateCommon.hpp>

namespace db::query::events
{
    class GetFiltered : public Query
    {
      public:
        GetFiltered(calendar::TimePoint filter_from,
                    calendar::TimePoint filter_till,
                    uint32_t offset = 0,
                    uint32_t limit  = UINT32_MAX);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        calendar::TimePoint filter_from;
        calendar::TimePoint filter_till;
        uint32_t offset;
        uint32_t limit;
    };

    class GetFilteredResult : public QueryResult
    {
        std::vector<EventsRecord> records;
        uint32_t recordsCount;

      public:
        GetFilteredResult(std::vector<EventsRecord> records, uint32_t count);
        auto getCountResult() -> uint32_t;
        [[nodiscard]] auto getResult() -> std::vector<EventsRecord>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
