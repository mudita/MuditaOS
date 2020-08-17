#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class Edit : public Query
    {
        EventsRecord record;
        /// Check value
        uint32_t dateFrom;

      public:
        Edit(EventsRecord record, const uint32_t &dateFrom);

        [[nodiscard]] auto getDateFrom() const -> uint32_t;
        [[nodiscard]] auto getRecord() const -> EventsRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class EditResult : public QueryResult
    {
      public:
        EditResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;

        const bool ret = true;
    };

} // namespace db::query::events
