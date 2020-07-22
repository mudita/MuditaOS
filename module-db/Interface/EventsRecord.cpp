#include "EventsRecord.hpp"
#include "module-db/queries/calendar/QueryEventsGet.hpp"
#include "module-db/queries/calendar/QueryEventsGetAll.hpp"
#include "module-db/queries/calendar/QueryEventsAdd.hpp"
//#include "module-db/queries/calendar/QueryEventsRemove.hpp"
//#include "module-db/queries/calendar/QueryEventsUpdate.hpp"

#include <log/log.hpp>
#include <Utils.hpp>

#include <cassert>
#include <vector>

EventsRecord::EventsRecord(const EventsTableRow &tableRow)
    : Record{tableRow.ID}, title{tableRow.title}, description{tableRow.description}, date_from{tableRow.date_from},
      date_till{tableRow.date_till}, reminder{tableRow.reminder}, repeat{tableRow.repeat}
{
    /// TODO:
    //     if (tableRow.key > static_cast<uint32_t>(Key::NotValidKey) &&
    //         tableRow.key < static_cast<uint32_t>(Key::NumberOfKeys)) {
    //         key = static_cast<Key>(tableRow.key);
    //         return;
    //     }
    //     LOG_ERROR("Invalid record");
    //     key = Key::NotValidKey;
    //     ID  = DB_ID_NONE;
}

EventsRecordInterface::EventsRecordInterface(EventsDB *eventsDb) : eventsDb(eventsDb)
{}

bool EventsRecordInterface::Add(const EventsRecord &rec)
{
    return eventsDb->events.add(EventsTableRow{{.ID = rec.ID},
                                               .title       = rec.title,
                                               .description = rec.description,
                                               .date_from   = rec.date_from,
                                               .date_till   = rec.date_till,
                                               .reminder    = rec.reminder,
                                               .repeat      = rec.repeat});

    return true;
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
    assert(0 && "Not implemented");

    if (limit == 0) {
        limit = GetCount();
    }

    auto rows = eventsDb->events.getLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<EventsRecord>>();

    //    for (auto &r : rows) {
    //        records->push_back(r);
    //    }

    return records;
}

bool EventsRecordInterface::Update(const EventsRecord &rec)
{
    auto entry = eventsDb->events.getById(rec.ID);
    if (!entry.isValid() || entry.date_from != static_cast<uint32_t>(rec.date_from)) {
        return false;
    }

    return eventsDb->events.update(EventsTableRow{{.ID = rec.ID},
                                                  .title       = rec.title,
                                                  .description = rec.description,
                                                  .date_from   = rec.date_from,
                                                  .date_till   = rec.date_till,
                                                  .reminder    = rec.reminder,
                                                  .repeat      = rec.repeat});
}

bool EventsRecordInterface::RemoveByID(uint32_t id)
{
    assert(0 && "Not implemented");

    return false;
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

// EventsRecord EventsRecordInterface::GetByKey(EventsRecord::Key key)
//{
//    if (!EventsRecord::isValidKey(key)) {
//        return EventsRecord();
//    }
//
//    return eventsDb->events.GetByKey(static_cast<uint32_t>(key));
//}

std::unique_ptr<db::QueryResult> EventsRecordInterface::runQuery(const db::Query *query)
{
    /// TODO: change to switch case or visitor pattern
    //    switch (query) {
    //        case db::query::events::GetResult:
    //            runQueryImpl(query);
    //        case db::query::events::GetAllResult:
    //            runQueryImpl(query);
    //        case db::query::events::Add:
    //            runQueryImpl(query);
    //        case db::query::events::Remove:
    //            runQueryImpl(query);
    //        case db::query::events::Update:
    //            runQueryImpl(query);
    //        case db::query::events::UpdateAll:
    //            runQueryImpl(query);
    //    }
    //    return nullptr;

    if (const auto local_query = dynamic_cast<const db::query::events::Get *>(query)) {
        return runQueryImpl(local_query);
    }
    //    if (const auto local_query = dynamic_cast<const db::query::events::Add *>(query)) {
    //        return runQueryImpl(local_query);
    //    }
    //    if (const auto local_query = dynamic_cast<const db::query::events::Remove *>(query)) {
    //        return runQueryImpl(local_query);
    //    }
    if (const auto local_query = dynamic_cast<const db::query::events::GetAll *>(query)) {
        return runQueryImpl(local_query);
    }
    //    if (const auto local_query = dynamic_cast<const db::query::events::Update *>(query)) {
    //        return runQueryImpl(local_query);
    //    }
    return nullptr;
}

std::unique_ptr<db::query::events::GetResult> EventsRecordInterface::runQueryImpl(const db::query::events::Get *query)
{
    auto value = GetByID(query->id);
    return std::make_unique<db::query::events::GetResult>(value);
}

// std::unique_ptr<db::query::events::IncrementResult> EventsRecordInterface::runQueryImpl(
//    const db::query::events::Increment *query)
//{
//    auto ret = false;
//
//    auto record = GetByKey(query->key);
//    if (record.isValid() && record.key == query->key) {
//        record.value++;
//        ret = Update(record);
//    }
//    return std::make_unique<db::query::events::IncrementResult>(ret);
//}
//
// std::unique_ptr<db::query::events::ClearResult> EventsRecordInterface::runQueryImpl(
//    const db::query::events::Clear *query)
//{
//    auto ret = false;
//
//    auto record = GetByKey(query->key);
//    if (record.isValid() && record.key == query->key) {
//        record.value = 0;
//        ret          = Update(record);
//    }
//    return std::make_unique<db::query::events::ClearResult>(ret);
//}

std::unique_ptr<db::query::events::GetAllResult> EventsRecordInterface::runQueryImpl(
    const db::query::events::GetAll *query)
{
    auto numberOfEvents = GetCount();
    auto records        = GetLimitOffset(0, numberOfEvents);
    return std::make_unique<db::query::events::GetAllResult>(std::move(records));
}
