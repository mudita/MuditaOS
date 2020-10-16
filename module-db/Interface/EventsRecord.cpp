#include "EventsRecord.hpp"
#include "module-db/queries/calendar/QueryEventsGet.hpp"
#include "module-db/queries/calendar/QueryEventsGetAll.hpp"
#include "module-db/queries/calendar/QueryEventsAdd.hpp"
#include "module-db/queries/calendar/QueryEventsRemove.hpp"
#include "module-db/queries/calendar/QueryEventsEdit.hpp"
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-db/queries/calendar/QueryEventsGetAllLimited.hpp>
#include <module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp>
#include <log/log.hpp>
#include <Utils.hpp>
#include <cassert>
#include <vector>

EventsRecord::EventsRecord(const EventsTableRow &tableRow)
    : Record{tableRow.ID}, title{tableRow.title}, date_from{tableRow.date_from}, date_till{tableRow.date_till},
      reminder{tableRow.reminder}, repeat{tableRow.repeat}, reminder_fired{tableRow.reminder_fired}
{}

EventsRecordInterface::EventsRecordInterface(EventsDB *eventsDb) : eventsDb(eventsDb)
{}

bool EventsRecordInterface::Add(const EventsRecord &rec)
{
    auto entry = EventsTableRow{{.ID = rec.ID},
                                .title          = rec.title,
                                .date_from      = rec.date_from,
                                .date_till      = rec.date_till,
                                .reminder       = rec.reminder,
                                .repeat         = rec.repeat,
                                .reminder_fired = rec.reminder_fired};

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
        return eventsDb->events.addCustom(entry);
    }
    }
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
                                .title          = rec.title,
                                .date_from      = rec.date_from,
                                .date_till      = rec.date_till,
                                .reminder       = rec.reminder,
                                .repeat         = rec.repeat,
                                .reminder_fired = rec.reminder_fired};

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
        return eventsDb->events.addCustom(entry);
    }
    }
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

std::unique_ptr<std::vector<EventsRecord>> EventsRecordInterface::SelectFirstUpcoming(TimePoint filter_from,
                                                                                      TimePoint filter_till)
{
    auto rows = eventsDb->events.SelectFirstUpcoming(filter_from, filter_till);

    auto records = std::make_unique<std::vector<EventsRecord>>();
    for (auto &r : rows) {
        records->push_back(r);
    }

    return records;
}

std::unique_ptr<db::QueryResult> EventsRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::events::Get)) {
        return runQueryImplGetResult(query);
    }
    if (typeid(*query) == typeid(db::query::events::GetAll)) {
        return runQueryImplGetAllResult(query);
    }
    if (typeid(*query) == typeid(db::query::events::GetAllLimited)) {
        return runQueryImplGetAllLimitedResult(query);
    }
    if (typeid(*query) == typeid(db::query::events::GetFiltered)) {
        return runQueryImplGetFilteredResult(query);
    }
    if (typeid(*query) == typeid(db::query::events::Add)) {
        return runQueryImplAdd(query);
    }
    if (typeid(*query) == typeid(db::query::events::Remove)) {
        return runQueryImplRemove(query);
    }
    if (typeid(*query) == typeid(db::query::events::Edit)) {
        return runQueryImplEdit(query);
    }
    if (typeid(*query) == typeid(db::query::events::SelectFirstUpcoming)) {
        return runQueryImplSelectFirstUpcoming(query);
    }
    return nullptr;
}

std::unique_ptr<db::query::events::GetResult> EventsRecordInterface::runQueryImplGetResult(
    std::shared_ptr<db::Query> query)
{
    auto getQuery = dynamic_cast<db::query::events::Get *>(query.get());
    assert(getQuery != nullptr);
    auto records  = GetByID(getQuery->id);
    auto response = std::make_unique<db::query::events::GetResult>(records);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::GetAllResult> EventsRecordInterface::runQueryImplGetAllResult(
    std::shared_ptr<db::Query> query)
{
    auto numberOfEvents = GetCount();
    auto records        = GetLimitOffset(0, numberOfEvents);
    auto response       = std::make_unique<db::query::events::GetAllResult>(std::move(records));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::GetAllLimitedResult> EventsRecordInterface::runQueryImplGetAllLimitedResult(
    std::shared_ptr<db::Query> query)
{

    auto getAllLimitedQuery = dynamic_cast<db::query::events::GetAllLimited *>(query.get());
    assert(getAllLimitedQuery != nullptr);
    auto records = GetLimitOffsetByDate(getAllLimitedQuery->offset, getAllLimitedQuery->limit);
    auto count   = GetCount();
    auto response =
        std::make_unique<db::query::events::GetAllLimitedResult>(std::move(records), std::make_unique<uint32_t>(count));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::GetFilteredResult> EventsRecordInterface::runQueryImplGetFilteredResult(
    std::shared_ptr<db::Query> query)
{
    auto getFilteredQuery = dynamic_cast<db::query::events::GetFiltered *>(query.get());
    assert(getFilteredQuery != nullptr);
    auto records  = Select(getFilteredQuery->filter_from, getFilteredQuery->filter_till);
    auto response = std::make_unique<db::query::events::GetFilteredResult>(std::move(records));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::AddResult> EventsRecordInterface::runQueryImplAdd(std::shared_ptr<db::Query> query)
{
    auto addQuery = dynamic_cast<db::query::events::Add *>(query.get());
    assert(addQuery != nullptr);
    bool ret      = Add(addQuery->getRecord());
    auto response = std::make_unique<db::query::events::AddResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::RemoveResult> EventsRecordInterface::runQueryImplRemove(
    std::shared_ptr<db::Query> query)
{
    auto removeQuery = dynamic_cast<db::query::events::Remove *>(query.get());
    assert(removeQuery != nullptr);
    bool ret      = RemoveByID(removeQuery->id);
    auto response = std::make_unique<db::query::events::RemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::EditResult> EventsRecordInterface::runQueryImplEdit(std::shared_ptr<db::Query> query)
{
    auto editQuery = dynamic_cast<db::query::events::Edit *>(query.get());
    assert(editQuery != nullptr);
    bool ret      = Update(editQuery->getRecord());
    auto response = std::make_unique<db::query::events::EditResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::SelectFirstUpcomingResult> EventsRecordInterface::runQueryImplSelectFirstUpcoming(
    std::shared_ptr<db::Query> query)
{
    auto getFirstUpcomingQuery = dynamic_cast<db::query::events::SelectFirstUpcoming *>(query.get());
    assert(getFirstUpcomingQuery != nullptr);

    auto records  = SelectFirstUpcoming(getFirstUpcomingQuery->filter_from, getFirstUpcomingQuery->filter_till);
    auto response = std::make_unique<db::query::events::SelectFirstUpcomingResult>(std::move(records));
    response->setRequestQuery(query);
    return response;
}
