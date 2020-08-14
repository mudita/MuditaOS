#include "QueryEventsGetFiltered.hpp"

namespace db::query::events
{
    GetFiltered::GetFiltered(TimePoint filter_from, TimePoint filter_till)
        : Query(Query::Type::Read), filter_from(filter_from), filter_till(filter_till)
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
