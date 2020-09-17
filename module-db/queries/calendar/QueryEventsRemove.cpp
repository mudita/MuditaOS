#include "QueryEventsRemove.hpp"

namespace db::query::events
{
    Remove::Remove(const uint32_t &id) : Query(Query::Type::Delete), id(id)
    {}

    auto Remove::debugInfo() const -> std::string
    {
        return "Remove";
    }

    RemoveResult::RemoveResult(const bool &ret) : ret(ret)
    {}

    auto RemoveResult::getResult() const -> bool
    {
        return ret;
    }

    auto RemoveResult::debugInfo() const -> std::string
    {
        return "RemoveResult";
    }
} // namespace db::query::events
