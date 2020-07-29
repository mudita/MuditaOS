#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class GetFiltered : public Query
    {
      public:
        GetFiltered(const uint32_t &date_from, const uint32_t &date_till);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        uint32_t date_from;
        uint32_t date_till;
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
