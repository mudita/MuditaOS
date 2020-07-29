#include "EventsRecord.hpp"
#include "module-db/queries/calendar/QueryEventsGet.hpp"
#include "module-db/queries/calendar/QueryEventsGetAll.hpp"
#include "module-db/queries/calendar/QueryEventsAdd.hpp"
#include "module-db/queries/calendar/QueryEventsRemove.hpp"
#include "module-db/queries/calendar/QueryEventsEdit.hpp"
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <log/log.hpp>
#include <Utils.hpp>
#include <cassert>
#include <vector>

EventsRecord::EventsRecord(const EventsTableRow &tableRow)
    : Record{tableRow.ID}, title{tableRow.title}, description{tableRow.description}, date_from{tableRow.date_from},
      date_till{tableRow.date_till}, reminder{tableRow.reminder}, repeat{tableRow.repeat}, time_zone{tableRow.time_zone}
{}

EventsRecordInterface::EventsRecordInterface(EventsDB *eventsDb) : eventsDb(eventsDb)
{}

bool EventsRecordInterface::Add(const EventsRecord &rec)
{
    eventsDb->events.add(EventsTableRow{{.ID = rec.ID},
                                        .title       = rec.title,
                                        .description = rec.description,
                                        .date_from   = rec.date_from,
                                        .date_till   = rec.date_till,
                                        .reminder    = rec.reminder,
                                        .repeat      = rec.repeat,
                                        .time_zone   = rec.time_zone});

    return true;
}

std::unique_ptr<std::vector<EventsRecord>> EventsRecordInterface::Select(uint32_t from, uint32_t till)
{
    auto rows = eventsDb->events.selectByDatePeriod(from, till);

    auto records = std::make_unique<std::vector<EventsRecord>>();

    for (auto &r : rows) {
        records->push_back(r);
    }

    return records;
}

std::unique_ptr<std::vector<EventsRecord>> EventsRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                        uint32_t limit,
                                                                                        EventsRecordField field,
                                                                                        const char *str)
{
    assert(0 && "Not implemented");

    return GetLimitOffset(offset, limit);
}

std::unique_ptr<std::vector<EventsRecord>> EventsRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    if (limit == 0) {
        limit = GetCount();
    }

    auto rows = eventsDb->events.getLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<EventsRecord>>();

    for (auto &r : rows) {
        records->push_back(r);
    }

    return records;
}

bool EventsRecordInterface::Update(const EventsRecord &rec, const uint32_t &checkValue)
{
    auto entry = eventsDb->events.getById(rec.ID);
    if (!entry.isValid() || entry.date_from != checkValue) {
        LOG_DEBUG("IS NOT VALID");
        return false;
    }

    return eventsDb->events.update(EventsTableRow{{.ID = rec.ID},
                                                  .title       = rec.title,
                                                  .description = rec.description,
                                                  .date_from   = rec.date_from,
                                                  .date_till   = rec.date_till,
                                                  .reminder    = rec.reminder,
                                                  .repeat      = rec.repeat,
                                                  .time_zone   = rec.time_zone});
}

bool EventsRecordInterface::RemoveByID(uint32_t id)
{
    return eventsDb->events.removeById(id);
}

bool EventsRecordInterface::RemoveByField(EventsRecordField field, const char *str)
{
    assert(0 && "Not implemented");

    return false;
}

EventsRecord EventsRecordInterface::GetByID(uint32_t id)
{
    EventsRecord event = static_cast<EventsRecord>(eventsDb->events.getById(id));
    return event;
}

uint32_t EventsRecordInterface::GetCount()
{
    return eventsDb->events.count();
}

std::unique_ptr<db::QueryResult> EventsRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::events::Get)) {
        const auto local_query = dynamic_cast<const db::query::events::Get *>(query.get());
        return runQueryImpl(local_query);
    }
    if (typeid(*query) == typeid(db::query::events::GetAll)) {
        const auto local_query = dynamic_cast<const db::query::events::GetAll *>(query.get());
        return runQueryImpl(local_query);
    }
    if (typeid(*query) == typeid(db::query::events::GetFiltered)) {
        const auto local_query = dynamic_cast<const db::query::events::GetFiltered *>(query.get());
        return runQueryImpl(local_query);
    }
    if (typeid(*query) == typeid(db::query::events::Add)) {
        const auto local_query = dynamic_cast<const db::query::events::Add *>(query.get());
        return runQueryImpl(local_query);
    }
    if (typeid(*query) == typeid(db::query::events::Remove)) {
        const auto local_query = dynamic_cast<const db::query::events::Remove *>(query.get());
        return runQueryImpl(local_query);
    }
    if (typeid(*query) == typeid(db::query::events::Edit)) {
        const auto local_query = dynamic_cast<const db::query::events::Edit *>(query.get());
        return runQueryImpl(local_query);
    }
    return nullptr;
}

std::unique_ptr<db::query::events::GetResult> EventsRecordInterface::runQueryImpl(const db::query::events::Get *query)
{
    auto value = GetByID(query->id);
    return std::make_unique<db::query::events::GetResult>(value);
}

std::unique_ptr<db::query::events::GetAllResult> EventsRecordInterface::runQueryImpl(
    const db::query::events::GetAll *query)
{
    auto numberOfEvents = GetCount();
    auto records        = GetLimitOffset(0, numberOfEvents);
    return std::make_unique<db::query::events::GetAllResult>(std::move(records));
}

std::unique_ptr<db::query::events::GetFilteredResult> EventsRecordInterface::runQueryImpl(
    const db::query::events::GetFiltered *query)
{
    auto records = Select(query->date_from, query->date_till);
    return std::make_unique<db::query::events::GetFilteredResult>(std::move(records));
}

std::unique_ptr<db::query::events::AddResult> EventsRecordInterface::runQueryImpl(const db::query::events::Add *query)
{
    bool ret = Add(query->getRecord());
    return std::make_unique<db::query::events::AddResult>(ret);
}

std::unique_ptr<db::query::events::RemoveResult> EventsRecordInterface::runQueryImpl(
    const db::query::events::Remove *query)
{
    bool ret = RemoveByID(query->id);
    return std::make_unique<db::query::events::RemoveResult>(ret);
}

std::unique_ptr<db::query::events::EditResult> EventsRecordInterface::runQueryImpl(const db::query::events::Edit *query)
{
    bool ret = Update(query->getRecord(), query->getDateFrom());
    return std::make_unique<db::query::events::EditResult>(ret);
}
