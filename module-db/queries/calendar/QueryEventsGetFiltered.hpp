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
        GetFiltered(TimePoint filter_from, TimePoint filter_till);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        TimePoint filter_from;
        TimePoint filter_till;
    };

    class GetFilteredResult : public QueryResult
    {
        std::unique_ptr<std::vector<EventsRecord>> records;

      public:
        GetFilteredResult(std::unique_ptr<std::vector<EventsRecord>> records);
        [[nodiscard]] auto getResult() -> std::unique_ptr<std::vector<EventsRecord>>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
