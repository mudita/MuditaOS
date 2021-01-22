// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>
#include <module-apps/application-calendar/data/dateCommon.hpp>

namespace db::query::events
{
    /// Query for getting calendar event whose reminder is first in filtered time range
    class SelectFirstUpcoming : public Query
    {
      public:
        SelectFirstUpcoming(calendar::TimePoint filter_from, calendar::TimePoint filter_till);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        calendar::TimePoint filter_from;
        calendar::TimePoint filter_till;
    };

    /// Result of SelectFirstUpcoming query
    class SelectFirstUpcomingResult : public QueryResult
    {
        std::vector<EventsRecord> records;

      public:
        SelectFirstUpcomingResult(std::vector<EventsRecord> records);
        [[nodiscard]] auto getResult() -> std::vector<EventsRecord>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
