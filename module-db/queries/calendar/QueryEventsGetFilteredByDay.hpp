// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>
#include <module-apps/application-calendar/data/dateCommon.hpp>

namespace db::query::events
{
    class GetFilteredByDay : public Query
    {
      public:
        GetFilteredByDay(TimePoint filterDate, uint32_t offset = 0, uint32_t limit = UINT32_MAX);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        TimePoint filterDate;
        uint32_t offset;
        uint32_t limit;
    };

    class GetFilteredByDayResult : public QueryResult
    {
        std::vector<EventsRecord> records;
        uint32_t recordsCount;

      public:
        GetFilteredByDayResult(const std::vector<EventsRecord> &records, uint32_t count);
        auto getCountResult() -> uint32_t;
        [[nodiscard]] auto getResult() -> std::vector<EventsRecord>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
