#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class Add : public Query
    {
        EventsRecord record;

      public:
        Add(EventsRecord record);
        [[nodiscard]] auto getRecord() const -> EventsRecord;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class AddResult : public QueryResult
    {
      public:
        AddResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;

        const bool ret = true;
    };

} // namespace db::query::events
