#include "QueryEventsGetFiltered.hpp"

namespace db::query::events
{
    GetFiltered::GetFiltered(const uint32_t &date_from, const uint32_t &date_till)
        : Query(Query::Type::Read), date_from(date_from), date_till(date_till)
    {}

    auto GetFiltered::debugInfo() const -> std::string
    {
        return "GetFiltered";
    }

    GetFilteredResult::GetFilteredResult(std::unique_ptr<std::vector<EventsRecord>> records)
        : records(std::move(records))
    {}

    auto GetFilteredResult::getResult() -> std::unique_ptr<std::vector<EventsRecord>>
    {
        return std::move(records);
    }

    auto GetFilteredResult::debugInfo() const -> std::string
    {
        return "GetFilteredResult";
    }
} // namespace db::query::events
