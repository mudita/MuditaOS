#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class GetAll : public Query
    {
      public:
        GetAll();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetAllResult : public QueryResult
    {
        std::unique_ptr<std::vector<EventsRecord>> records;

      public:
        GetAllResult(std::unique_ptr<std::vector<EventsRecord>> records);
        [[nodiscard]] auto getResult() -> std::unique_ptr<std::vector<EventsRecord>>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
