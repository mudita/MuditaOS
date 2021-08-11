// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmEventRecord.hpp"

#include <Databases/EventsDB.hpp>
#include <queries/alarm_events/QueryAlarmEventsAdd.hpp>
#include <queries/alarm_events/QueryAlarmEventsEdit.hpp>
#include <queries/alarm_events/QueryAlarmEventsGet.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetRecurringBetweenDates.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetLimited.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetBetweenDates.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetNext.hpp>
#include <queries/alarm_events/QueryAlarmEventsRemove.hpp>

#include <rrule/rrule.hpp>

AlarmEventRecord::AlarmEventRecord(uint32_t id,
                                   UTF8 name,
                                   TimePoint startDate,
                                   TimePoint endDate,
                                   uint32_t duration,
                                   bool isAllDay,
                                   std::string rrule,
                                   std::string musicTone,
                                   bool enabled,
                                   uint32_t snoozeDuration)
    : EventRecord{id, name, startDate, endDate, duration, isAllDay, rrule}, musicTone{musicTone}, enabled{enabled},
      snoozeDuration{snoozeDuration} {};

AlarmEventRecord::AlarmEventRecord(const AlarmEventsTableRow &aeRow)
    : EventRecord{aeRow.ID, aeRow.name, aeRow.startDate, aeRow.endDate, aeRow.duration, aeRow.isAllDay, aeRow.rrule},
      musicTone{aeRow.musicTone}, enabled{aeRow.enabled}, snoozeDuration{aeRow.snoozeDuration} {};

AlarmEventRecordInterface::AlarmEventRecordInterface(EventsDB *eventsDB) : eventsDB(eventsDB)
{}

AlarmEventRecordInterface::~AlarmEventRecordInterface()
{}

std::unique_ptr<db::QueryResult> AlarmEventRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::alarmEvents::Add)) {
        return runQueryImplAdd(std::static_pointer_cast<db::query::alarmEvents::Add>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::Edit)) {
        return runQueryImplEdit(std::static_pointer_cast<db::query::alarmEvents::Edit>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::Get)) {
        return runQueryImplGet(std::static_pointer_cast<db::query::alarmEvents::Get>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::GetRecurringBetweenDates)) {
        return runQueryImplGetRecurringBetweenDates(
            std::static_pointer_cast<db::query::alarmEvents::GetRecurringBetweenDates>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::GetLimited)) {
        return runQueryImplGetLimited(std::static_pointer_cast<db::query::alarmEvents::GetLimited>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::GetBetweenDates)) {
        return runQueryImplGetBetweenDates(std::static_pointer_cast<db::query::alarmEvents::GetBetweenDates>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::GetNext)) {
        return runQueryImplGetNext(std::static_pointer_cast<db::query::alarmEvents::GetNext>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::Remove)) {
        return runQueryImplRemove(std::static_pointer_cast<db::query::alarmEvents::Remove>(query));
    }
    return nullptr;
}

std::unique_ptr<db::query::alarmEvents::AddResult> AlarmEventRecordInterface::runQueryImplAdd(
    std::shared_ptr<db::query::alarmEvents::Add> query)
{
    eventsDB->alarmEvents.add(AlarmEventsTableRow(query->getRecord()));

    auto response = std::make_unique<db::query::alarmEvents::AddResult>(true);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarmEvents::EditResult> AlarmEventRecordInterface::runQueryImplEdit(
    std::shared_ptr<db::query::alarmEvents::Edit> query)
{
    eventsDB->alarmEvents.update(AlarmEventsTableRow(query->getRecord()));

    auto response = std::make_unique<db::query::alarmEvents::EditResult>(true);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarmEvents::GetResult> AlarmEventRecordInterface::runQueryImplGet(
    std::shared_ptr<db::query::alarmEvents::Get> query)
{
    auto record = eventsDB->alarmEvents.getById(query->id);

    auto response = std::make_unique<db::query::alarmEvents::GetResult>(AlarmEventRecord(record));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarmEvents::GetLimitedResult> AlarmEventRecordInterface::runQueryImplGetLimited(
    std::shared_ptr<db::query::alarmEvents::GetLimited> query)
{
    auto alarmEventsRows = eventsDB->alarmEvents.getLimitOffset(query->offset, query->limit);

    std::vector<AlarmEventRecord> recordVector;

    for (const auto &row : alarmEventsRows) {
        recordVector.push_back(AlarmEventRecord(row));
    }

    auto response = std::make_unique<db::query::alarmEvents::GetLimitedResult>(recordVector);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::GetBetweenDatesResult> AlarmEventRecordInterface::runQueryImplGetBetweenDates(
    std::shared_ptr<db::query::alarmEvents::GetBetweenDates> query)
{
    auto alarmEventsRows = eventsDB->alarmEvents.getBetweenDates(query->start, query->end, query->offset, query->limit);

    std::vector<AlarmEventRecord> recordVector;

    for (const auto &row : alarmEventsRows) {
        recordVector.push_back(AlarmEventRecord(row));
    }

    auto response = std::make_unique<db::query::alarmEvents::GetBetweenDatesResult>(recordVector);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::GetRecurringBetweenDatesResult> AlarmEventRecordInterface::
    runQueryImplGetRecurringBetweenDates(std::shared_ptr<db::query::alarmEvents::GetRecurringBetweenDates> query)
{
    auto alarmEventsRows =
        eventsDB->alarmEvents.getRecurringBetweenDates(query->start, query->end, query->offset, query->limit);

    std::vector<AlarmEventRecord> recordVector;

    for (const auto &row : alarmEventsRows) {
        recordVector.push_back(AlarmEventRecord(row));
    }

    auto response = std::make_unique<db::query::alarmEvents::GetRecurringBetweenDatesResult>(recordVector);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::GetNextResult> AlarmEventRecordInterface::runQueryImplGetNext(
    std::shared_ptr<db::query::alarmEvents::GetNext> query)
{
    auto alarmEventsRows = eventsDB->alarmEvents.getNext(query->start, query->offset, query->limit);

    std::vector<AlarmEventRecord> recordVector;

    for (const auto &row : alarmEventsRows) {
        recordVector.push_back(AlarmEventRecord(row));
    }

    auto response = std::make_unique<db::query::alarmEvents::GetNextResult>(recordVector);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::RemoveResult> AlarmEventRecordInterface::runQueryImplRemove(
    std::shared_ptr<db::query::alarmEvents::Remove> query)
{
    bool ret      = eventsDB->alarmEvents.removeById(query->id);
    auto response = std::make_unique<db::query::alarmEvents::RemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}
