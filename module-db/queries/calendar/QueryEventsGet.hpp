#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class Get : public Query
    {
      public:
        const uint32_t id;
        Get(const uint32_t &id);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetResult : public QueryResult
    {
        EventsRecord record;

      public:
        GetResult(EventsRecord record);
        [[nodiscard]] auto getResult() const -> EventsRecord;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
