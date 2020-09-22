#pragma once

#include <module-db/Interface/EventsRecord.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class GetAllLimited : public Query
    {
      public:
        GetAllLimited(const uint32_t &offset, const uint32_t &limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        uint32_t offset = 0;
        uint32_t limit  = 0;
    };

    class GetAllLimitedResult : public QueryResult
    {
        std::unique_ptr<std::vector<EventsRecord>> records;
        std::unique_ptr<uint32_t> recordsCount;

      public:
        GetAllLimitedResult(std::unique_ptr<std::vector<EventsRecord>> records, std::unique_ptr<uint32_t> count);
        [[nodiscard]] auto getResult() -> std::unique_ptr<std::vector<EventsRecord>>;
        [[nodiscard]] auto getCountResult() -> std::unique_ptr<uint32_t>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
