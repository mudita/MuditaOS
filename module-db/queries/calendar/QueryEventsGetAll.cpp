#include "QueryEventsGetAll.hpp"

namespace db::query::events
{
    GetAll::GetAll() : Query(Query::Type::Read)
    {}

    auto GetAll::debugInfo() const -> std::string
    {
        return "GetAll";
    }

    GetAllResult::GetAllResult(std::unique_ptr<std::vector<EventsRecord>> records) : records(std::move(records))
    {}

    auto GetAllResult::getResult() -> std::unique_ptr<std::vector<EventsRecord>>
    {
        return std::move(records);
    }

    auto GetAllResult::debugInfo() const -> std::string
    {
        return "GetAllResult";
    }
} // namespace db::query::events
