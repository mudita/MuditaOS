#include "QueryEventsEdit.hpp"

namespace db::query::events
{
    Edit::Edit(EventsRecord record, const uint32_t &dateFrom)
        : Query(Query::Type::Update), record(record), dateFrom(dateFrom)
    {}

    auto Edit::getRecord() const -> EventsRecord
    {
        return record;
    }

    auto Edit::getDateFrom() const -> uint32_t
    {
        return dateFrom;
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
