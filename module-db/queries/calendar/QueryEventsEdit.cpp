#include "QueryEventsEdit.hpp"

namespace db::query::events
{
    Edit::Edit(EventsRecord record) : Query(Query::Type::Update), record(record)
    {}

    auto Edit::getRecord() const -> EventsRecord
    {
        return record;
    }

    auto Edit::debugInfo() const -> std::string
    {
        return "Edit";
    }

    EditResult::EditResult(bool ret) : ret(ret)
    {}

    auto EditResult::getResult() const -> bool
    {
        return ret;
    }

    auto EditResult::debugInfo() const -> std::string
    {
        return "EditResult";
    }
} // namespace db::query::events
