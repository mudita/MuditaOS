// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventsRecord.hpp"
#include "module-db/queries/calendar/QueryEventsGet.hpp"
#include "module-db/queries/calendar/QueryEventsGetAll.hpp"
#include "module-db/queries/calendar/QueryEventsAdd.hpp"
#include "module-db/queries/calendar/QueryEventsRemove.hpp"
#include "module-db/queries/calendar/QueryEventsRemoveICS.hpp"
#include "module-db/queries/calendar/QueryEventsEdit.hpp"
#include <module-db/queries/calendar/QueryEventsEditICS.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-db/queries/calendar/QueryEventsGetFilteredByDay.hpp>
#include <module-db/queries/calendar/QueryEventsGetAllLimited.hpp>
#include <module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp>
#include <log.hpp>
#include <Utils.hpp>
#include <cassert>
#include <vector>

EventsRecord::EventsRecord(const EventsTableRow &tableRow)
    : Record{tableRow.ID}, UID{tableRow.UID}, title{tableRow.title}, date_from{tableRow.date_from},
      date_till{tableRow.date_till}, reminder{tableRow.reminder}, repeat{tableRow.repeat},
      reminder_fired{tableRow.reminder_fired}, provider_type{tableRow.provider_type}, provider_id{tableRow.provider_id},
      provider_iCalUid{tableRow.provider_iCalUid}
{}

EventsRecordInterface::EventsRecordInterface(EventsDB *eventsDb) : eventsDb(eventsDb)
{}

bool EventsRecordInterface::Add(const EventsRecord &rec)
{
    auto entry = EventsTableRow{{rec.ID},
                                .UID              = rec.UID,
                                .title            = rec.title,
                                .date_from        = rec.date_from,
                                .date_till        = rec.date_till,
                                .reminder         = rec.reminder,
                                .repeat           = rec.repeat,
                                .reminder_fired   = rec.reminder_fired,
                                .provider_type    = rec.provider_type,
                                .provider_id      = rec.provider_id,
                                .provider_iCalUid = rec.provider_iCalUid};

    Repeat repeatOption = Repeat(rec.repeat);
    if (repeatOption > Repeat::yearly) {
        return (eventsDb->events.addCustom(entry));
    }

    switch (repeatOption) {
    case Repeat::never: {
        return eventsDb->events.add(entry);
    }
    case Repeat::daily: {
        return eventsDb->events.addDaily(entry);
    }
    case Repeat::weekly: {
        return eventsDb->events.addWeekly(entry);
    }
    case Repeat::biweekly: {
        return eventsDb->events.addTwoWeeks(entry);
    }
    case Repeat::monthly: {
        return eventsDb->events.addMonth(entry);
    }
    case Repeat::yearly: {
        return eventsDb->events.addYear(entry);
    }
    }

    return false;
}

std::vector<EventsRecord> EventsRecordInterface::Select(TimePoint filter_from,
                                                        TimePoint filter_till,
                                                        uint32_t offset,
                                                        uint32_t limit)
{
    auto rows = eventsDb->events.selectByDatePeriod(filter_from, filter_till, offset, limit);

    auto records = std::vector<EventsRecord>();

    for (auto &r : rows) {
        records.push_back(r);
    }

    return records;
}

std::vector<EventsRecord> EventsRecordInterface::Select(TimePoint date, uint32_t offset, uint32_t limit)
{
    auto rows = eventsDb->events.selectByDate(date, offset, limit);

    auto records = std::vector<EventsRecord>();

    for (auto &r : rows) {
        records.push_back(r);
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

std::vector<EventsRecord> EventsRecordInterface::GetLimitOffsetByDate(uint32_t offset, uint32_t limit)
{
    if (limit == 0) {
        limit = GetCount();
    }

    auto rows = eventsDb->events.getLimitOffsetByDate(offset, limit);

    auto records = std::vector<EventsRecord>();

    for (auto &r : rows) {
        records.push_back(r);
    }

    return records;
}

bool EventsRecordInterface::Update(const EventsRecord &rec)
{
    auto record = eventsDb->events.getById(rec.ID);
    if (!record.isValid()) {
        LOG_DEBUG("Event record is not valid");
        return false;
    }

    auto entry = EventsTableRow{{rec.ID},
                                .UID              = rec.UID,
                                .title            = rec.title,
                                .date_from        = rec.date_from,
                                .date_till        = rec.date_till,
                                .reminder         = rec.reminder,
                                .repeat           = rec.repeat,
                                .reminder_fired   = rec.reminder_fired,
                                .provider_type    = rec.provider_type,
                                .provider_id      = rec.provider_id,
                                .provider_iCalUid = rec.provider_iCalUid};

    bool result = eventsDb->events.update(entry);

    Repeat repeatOption = Repeat(rec.repeat);
    if (repeatOption > Repeat::yearly) {
        return (eventsDb->events.addCustom(entry) && result);
    }

    switch (repeatOption) {
    case Repeat::never: {
        return result;
    }
    case Repeat::daily: {
        return (eventsDb->events.addDaily(entry) && result);
    }
    case Repeat::weekly: {
        return (eventsDb->events.addWeekly(entry) && result);
    }
    case Repeat::biweekly: {
        return (eventsDb->events.addTwoWeeks(entry) && result);
    }
    case Repeat::monthly: {
        return (eventsDb->events.addMonth(entry) && result);
    }
    case Repeat::yearly: {
        return (eventsDb->events.addYear(entry) && result);
    }
    }

    return false;
}

bool EventsRecordInterface::UpdateByUID(const EventsRecord &rec)
{
    auto record = eventsDb->events.getByUID(rec.UID);
    if (!record.isValid()) {
        LOG_DEBUG("Event record is not valid");
        return false;
    }

    auto entry = EventsTableRow{{rec.ID},
                                .UID              = rec.UID,
                                .title            = rec.title,
                                .date_from        = rec.date_from,
                                .date_till        = rec.date_till,
                                .reminder         = rec.reminder,
                                .repeat           = rec.repeat,
                                .reminder_fired   = rec.reminder_fired,
                                .provider_type    = rec.provider_type,
                                .provider_id      = rec.provider_id,
                                .provider_iCalUid = rec.provider_iCalUid};

    bool result = eventsDb->events.updateByUID(entry);

    Repeat repeatOption = Repeat(rec.repeat);
    if (repeatOption > Repeat::yearly) {
        return (eventsDb->events.addCustom(entry) && result);
    }

    switch (Repeat(rec.repeat)) {
    case Repeat::never: {
        return result;
    }
    case Repeat::daily: {
        return (eventsDb->events.addDaily(entry) && result);
    }
    case Repeat::weekly: {
        return (eventsDb->events.addWeekly(entry) && result);
    }
    case Repeat::biweekly: {
        return (eventsDb->events.addTwoWeeks(entry) && result);
    }
    case Repeat::monthly: {
        return (eventsDb->events.addMonth(entry) && result);
    }
    case Repeat::yearly: {
        return (eventsDb->events.addYear(entry) && result);
    }
    }

    return false;
}

bool EventsRecordInterface::RemoveByID(uint32_t id)
{
    return eventsDb->events.removeById(id);
}

bool EventsRecordInterface::RemoveByUID(const std::string &UID)
{
    return eventsDb->events.removeByUID(UID);
}

bool EventsRecordInterface::RemoveByField(EventsRecordField field, const char *str)
{
    assert(0 && "Not implemented");

    return false;
}

EventsRecord EventsRecordInterface::GetByID(uint32_t id)
{
    EventsRecord event{eventsDb->events.getById(id)};
    return event;
}

EventsRecord EventsRecordInterface::GetByUID(const std::string &uid)
{
    EventsRecord event{eventsDb->events.getByUID(uid)};
    return event;
}

uint32_t EventsRecordInterface::GetCount()
{
    return eventsDb->events.count();
}

uint32_t EventsRecordInterface::GetCountFiltered(TimePoint from, TimePoint till)
{
    return eventsDb->events.countFromFilter(from, till);
}

uint32_t EventsRecordInterface::GetCountFilteredByDay(TimePoint filter)
{
    return eventsDb->events.countFromDayFilter(filter);
}

std::vector<EventsRecord> EventsRecordInterface::SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till)
{
    auto rows = eventsDb->events.SelectFirstUpcoming(filter_from, filter_till);

    auto records = std::vector<EventsRecord>();
    for (auto &r : rows) {
        records.push_back(r);
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
    if (typeid(*query) == typeid(db::query::events::GetFilteredByDay)) {
        return runQueryImplGetFilteredByDayResult(query);
    }
    if (typeid(*query) == typeid(db::query::events::Add)) {
        return runQueryImplAdd(query);
    }
    if (typeid(*query) == typeid(db::query::events::Remove)) {
        return runQueryImplRemove(query);
    }
    if (typeid(*query) == typeid(db::query::events::RemoveICS)) {
        return runQueryImplRemoveICS(query);
    }
    if (typeid(*query) == typeid(db::query::events::Edit)) {
        return runQueryImplEdit(query);
    }
    if (typeid(*query) == typeid(db::query::events::EditICS)) {
        return runQueryImplEditICS(query);
    }
    if (typeid(*query) == typeid(db::query::events::SelectFirstUpcoming)) {
        return runQueryImplSelectFirstUpcoming(query);
    }
    return nullptr;
}

std::unique_ptr<db::query::events::GetResult> EventsRecordInterface::runQueryImplGetResult(
    std::shared_ptr<db::Query> query)
{
    auto getQuery = static_cast<db::query::events::Get *>(query.get());
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
    auto response       = std::make_unique<db::query::events::GetAllResult>(*records);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::GetAllLimitedResult> EventsRecordInterface::runQueryImplGetAllLimitedResult(
    std::shared_ptr<db::Query> query)
{

    auto getAllLimitedQuery = static_cast<db::query::events::GetAllLimited *>(query.get());
    auto records            = GetLimitOffsetByDate(getAllLimitedQuery->offset, getAllLimitedQuery->limit);
    auto count              = GetCount();
    auto response           = std::make_unique<db::query::events::GetAllLimitedResult>(std::move(records), count);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::GetFilteredResult> EventsRecordInterface::runQueryImplGetFilteredResult(
    std::shared_ptr<db::Query> query)
{
    auto getFilteredQuery = static_cast<db::query::events::GetFiltered *>(query.get());

    auto records = Select(getFilteredQuery->filter_from,
                          getFilteredQuery->filter_till,
                          getFilteredQuery->offset,
                          getFilteredQuery->limit);

    auto numberOfEvents = GetCountFiltered(getFilteredQuery->filter_from, getFilteredQuery->filter_till);
    auto response       = std::make_unique<db::query::events::GetFilteredResult>(std::move(records), numberOfEvents);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::GetFilteredByDayResult> EventsRecordInterface::runQueryImplGetFilteredByDayResult(
    std::shared_ptr<db::Query> query)
{
    auto getFilteredQuery = static_cast<db::query::events::GetFilteredByDay *>(query.get());

    auto records = Select(getFilteredQuery->filterDate, getFilteredQuery->offset, getFilteredQuery->limit);

    auto numberOfEvents = GetCountFilteredByDay(getFilteredQuery->filterDate);
    auto response = std::make_unique<db::query::events::GetFilteredByDayResult>(std::move(records), numberOfEvents);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::AddResult> EventsRecordInterface::runQueryImplAdd(std::shared_ptr<db::Query> query)
{
    auto addQuery = static_cast<db::query::events::Add *>(query.get());
    bool ret      = Add(addQuery->getRecord());
    auto response = std::make_unique<db::query::events::AddResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::RemoveResult> EventsRecordInterface::runQueryImplRemove(
    std::shared_ptr<db::Query> query)
{
    auto removeQuery = static_cast<db::query::events::Remove *>(query.get());
    bool ret         = RemoveByID(removeQuery->id);
    auto response    = std::make_unique<db::query::events::RemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::RemoveICSResult> EventsRecordInterface::runQueryImplRemoveICS(
    std::shared_ptr<db::Query> query)
{
    auto removeQuery = static_cast<db::query::events::RemoveICS *>(query.get());
    bool ret         = RemoveByUID(removeQuery->UID);
    auto response    = std::make_unique<db::query::events::RemoveICSResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::EditResult> EventsRecordInterface::runQueryImplEdit(std::shared_ptr<db::Query> query)
{
    auto editQuery = static_cast<db::query::events::Edit *>(query.get());
    bool ret       = Update(editQuery->getRecord());
    auto response  = std::make_unique<db::query::events::EditResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::EditICSResult> EventsRecordInterface::runQueryImplEditICS(
    std::shared_ptr<db::Query> query)
{
    auto editQuery = static_cast<db::query::events::EditICS *>(query.get());
    bool ret       = UpdateByUID(editQuery->getRecord());
    auto response  = std::make_unique<db::query::events::EditICSResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::events::SelectFirstUpcomingResult> EventsRecordInterface::runQueryImplSelectFirstUpcoming(
    std::shared_ptr<db::Query> query)
{
    auto getFirstUpcomingQuery = static_cast<db::query::events::SelectFirstUpcoming *>(query.get());

    auto records  = SelectFirstUpcoming(getFirstUpcomingQuery->filter_from, getFirstUpcomingQuery->filter_till);
    auto response = std::make_unique<db::query::events::SelectFirstUpcomingResult>(records);
    response->setRequestQuery(query);
    return response;
}
