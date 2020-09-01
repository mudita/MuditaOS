#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>
#include <module-apps/application-calendar/data/dateCommon.hpp>

namespace db::query::events
{
    class SelectFirstUpcoming : public Query
    {
      public:
        SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        TimePoint filter_from;
        TimePoint filter_till;
    };

    class SelectFirstUpcomingResult : public QueryResult
    {
        std::unique_ptr<std::vector<EventsRecord>> records;

      public:
        SelectFirstUpcomingResult(std::unique_ptr<std::vector<EventsRecord>> records);
        [[nodiscard]] auto getResult() -> std::unique_ptr<std::vector<EventsRecord>>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
