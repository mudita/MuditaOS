#include "EventsRecord.hpp"
#include "module-db/queries/calendar/QueryEventsGet.hpp"
#include "module-db/queries/calendar/QueryEventsGetAll.hpp"
#include "module-db/queries/calendar/QueryEventsAdd.hpp"
#include "module-db/queries/calendar/QueryEventsRemove.hpp"
#include "module-db/queries/calendar/QueryEventsEdit.hpp"
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-db/queries/calendar/QueryEventsGetAllLimited.hpp>
#include <log/log.hpp>
#include <Utils.hpp>
#include <cassert>
#include <vector>

EventsRecord::EventsRecord(const EventsTableRow &tableRow)
    : Record{tableRow.ID}, title{tableRow.title}, date_from{tableRow.date_from}, date_till{tableRow.date_till},
      reminder{tableRow.reminder}, repeat{tableRow.repeat}
{}

EventsRecordInterface::EventsRecordInterface(EventsDB *eventsDb) : eventsDb(eventsDb)
{}

bool EventsRecordInterface::Add(const EventsRecord &rec)
{
    auto entry = EventsTableRow{{.ID = rec.ID},
                                .title     = rec.title,
                                .date_from = rec.date_from,
                                .date_till = rec.date_till,
                                .reminder  = rec.reminder,
                                .repeat    = rec.repeat};

    switch (RepeatOption(rec.repeat)) {
    case RepeatOption::Never: {
        return eventsDb->events.add(entry);
    }
    case RepeatOption::Daily: {
        return eventsDb->events.addDaily(entry);
    }
    case RepeatOption::Weekly: {
        return eventsDb->events.addWeekly(entry);
    }
    case RepeatOption::TwoWeeks: {
        return eventsDb->events.addTwoWeeks(entry);
    }
    case RepeatOption::Month: {
        return eventsDb->events.addMonth(entry);
    }
    case RepeatOption::Year: {
        return eventsDb->events.addYear(entry);
    }
    default: {
        break;
        eventsDb->events.addCustom(entry);
    }
    }

    return true;
}

std::unique_ptr<std::vector<EventsRecord>> EventsRecordInterface::Select(TimePoint filter_from, TimePoint filter_till)
{
    auto rows = eventsDb->events.selectByDatePeriod(filter_from, filter_till);

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

std::unique_ptr<std::vector<EventsRecord>> EventsRecordInterface::GetLimitOffsetByDate(uint32_t offset, uint32_t limit)
{
    if (limit == 0) {
        limit = GetCount();
    }

    auto rows = eventsDb->events.getLimitOffsetByDate(offset, limit);

    auto records = std::make_unique<std::vector<EventsRecord>>();

    for (auto &r : rows) {
        records->push_back(r);
    }

    return records;
}

bool EventsRecordInterface::Update(const EventsRecord &rec)
{
    auto record = eventsDb->events.getById(rec.ID);
    if (!record.isValid()) {
        LOG_DEBUG("IS NOT VALID");
        return false;
    }

    auto entry = EventsTableRow{{.ID = rec.ID},
                                .title     = rec.title,
                                .date_from = rec.date_from,
                                .date_till = rec.date_till,
                                .reminder  = rec.reminder,
                                .repeat    = rec.repeat};

    bool result = eventsDb->events.update(entry);

    switch (RepeatOption(rec.repeat)) {
    case RepeatOption::Never: {
        return (eventsDb->events.add(entry) && result);
    }
    case RepeatOption::Daily: {
        return (eventsDb->events.addDaily(entry) && result);
    }
    case RepeatOption::Weekly: {
        return (eventsDb->events.addWeekly(entry) && result);
    }
    case RepeatOption::TwoWeeks: {
        return (eventsDb->events.addTwoWeeks(entry) && result);
    }
    case RepeatOption::Month: {
        return (eventsDb->events.addMonth(entry) && result);
    }
    case RepeatOption::Year: {
        return (eventsDb->events.addYear(entry) && result);
    }
    default: {
        break;
        eventsDb->events.addCustom(entry);
    }
    }
    return false;
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
    if (typeid(*query) == typeid(db::query::events::GetAllLimited)) {
        const auto local_query = dynamic_cast<const db::query::events::GetAllLimited *>(query.get());
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

std::unique_ptr<db::query::events::GetAllLimitedResult> EventsRecordInterface::runQueryImpl(
    const db::query::events::GetAllLimited *query)
{
    auto count   = GetCount();
    auto records = GetLimitOffsetByDate(query->offset, query->limit);
    return std::make_unique<db::query::events::GetAllLimitedResult>(std::move(records),
                                                                    std::make_unique<uint32_t>(count));
}

std::unique_ptr<db::query::events::GetFilteredResult> EventsRecordInterface::runQueryImpl(
    const db::query::events::GetFiltered *query)
{
    auto records = Select(query->filter_from, query->filter_till);
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
    bool ret = Update(query->getRecord());
    return std::make_unique<db::query::events::EditResult>(ret);
}
