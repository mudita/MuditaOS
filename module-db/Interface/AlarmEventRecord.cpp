// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmEventRecord.hpp"

#include <Databases/EventsDB.hpp>
#include <queries/alarm_events/QueryAlarmEventsAdd.hpp>
#include <queries/alarm_events/QueryAlarmEventsEdit.hpp>
#include <queries/alarm_events/QueryAlarmEventsGet.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetBetweenDates.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetLimited.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetNext.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetRecurringBetweenDates.hpp>
#include <queries/alarm_events/QueryAlarmEventsRemove.hpp>
#include <queries/alarm_events/QueryAlarmEventsToggleAll.hpp>

#include <rrule/rrule.hpp>

std::vector<AlarmEventRecord> generateRecordsVector(const std::vector<AlarmEventsTableRow> &tableRowVector);

AlarmEventRecord::AlarmEventRecord(uint32_t id,
                                   const UTF8 &name,
                                   TimePoint startDate,
                                   uint32_t duration,
                                   bool isAllDay,
                                   const std::string &rruleText,
                                   const std::string &musicTone,
                                   bool enabled,
                                   uint32_t snoozeDuration)
    : EventRecord{id, name, startDate, duration, isAllDay, rruleText}, musicTone{musicTone}, enabled{enabled},
      snoozeDuration{snoozeDuration} {};

AlarmEventRecord::AlarmEventRecord(const AlarmEventsTableRow &aeRow)
    : EventRecord{aeRow.ID, aeRow.name, aeRow.startDate, aeRow.duration, aeRow.isAllDay, aeRow.rruleText},
      musicTone{aeRow.musicTone}, enabled{aeRow.enabled}, snoozeDuration{aeRow.snoozeDuration} {};

auto AlarmEventRecord::isValid() const -> bool
{
    return (startDate != TIME_POINT_INVALID && ID != 0);
}

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
    if (typeid(*query) == typeid(db::query::alarmEvents::GetLimited)) {
        return runQueryImplGetLimited(std::static_pointer_cast<db::query::alarmEvents::GetLimited>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::GetBetweenDates)) {
        return runQueryImplGetBetweenDates(std::static_pointer_cast<db::query::alarmEvents::GetBetweenDates>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::GetNext)) {
        return runQueryImplGetNext(std::static_pointer_cast<db::query::alarmEvents::GetNext>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::GetRecurringBetweenDates)) {
        return runQueryImplGetRecurringBetweenDates(
            std::static_pointer_cast<db::query::alarmEvents::GetRecurringBetweenDates>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::Remove)) {
        return runQueryImplRemove(std::static_pointer_cast<db::query::alarmEvents::Remove>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::ToggleAll)) {
        return runQueryImplRemove(std::static_pointer_cast<db::query::alarmEvents::ToggleAll>(query));
    }
    return nullptr;
}

std::unique_ptr<db::query::alarmEvents::AddResult> AlarmEventRecordInterface::runQueryImplAdd(
    std::shared_ptr<db::query::alarmEvents::Add> query)
{
    const auto result = eventsDB->alarmEvents.add(AlarmEventsTableRow(query->getRecord()));

    auto response = std::make_unique<db::query::alarmEvents::AddResult>(result);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarmEvents::EditResult> AlarmEventRecordInterface::runQueryImplEdit(
    std::shared_ptr<db::query::alarmEvents::Edit> query)
{
    const auto result = eventsDB->alarmEvents.update(AlarmEventsTableRow(query->getRecord()));

    auto response = std::make_unique<db::query::alarmEvents::EditResult>(result);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarmEvents::GetResult> AlarmEventRecordInterface::runQueryImplGet(
    std::shared_ptr<db::query::alarmEvents::Get> query)
{
    const auto record = eventsDB->alarmEvents.getById(query->id);

    auto response = std::make_unique<db::query::alarmEvents::GetResult>(AlarmEventRecord(record));
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarmEvents::GetBetweenDatesResult> AlarmEventRecordInterface::runQueryImplGetBetweenDates(
    std::shared_ptr<db::query::alarmEvents::GetBetweenDates> query)
{
    const auto [alarmEventsRows, count] =
        eventsDB->alarmEvents.getBetweenDates(query->start, query->end, query->offset, query->limit);
    const auto recordVector = generateRecordsVector(alarmEventsRows);

    auto response = std::make_unique<db::query::alarmEvents::GetBetweenDatesResult>(recordVector, count);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::GetLimitedResult> AlarmEventRecordInterface::runQueryImplGetLimited(
    std::shared_ptr<db::query::alarmEvents::GetLimited> query)
{
    const auto alarmEventsRows = eventsDB->alarmEvents.getLimitOffset(query->offset, query->limit);
    const auto recordVector    = generateRecordsVector(alarmEventsRows);

    auto response = std::make_unique<db::query::alarmEvents::GetLimitedResult>(recordVector);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::GetNextResult> AlarmEventRecordInterface::runQueryImplGetNext(
    std::shared_ptr<db::query::alarmEvents::GetNext> query)
{
    const auto alarmEventsRows = eventsDB->alarmEvents.getNext(query->start, query->offset, query->limit);
    const auto recordVector    = generateRecordsVector(alarmEventsRows);

    auto response = std::make_unique<db::query::alarmEvents::GetNextResult>(recordVector);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::GetRecurringBetweenDatesResult> AlarmEventRecordInterface::
    runQueryImplGetRecurringBetweenDates(std::shared_ptr<db::query::alarmEvents::GetRecurringBetweenDates> query)
{
    const auto alarmEventsRows =
        eventsDB->alarmEvents.getRecurringBetweenDates(query->start, query->end, query->offset, query->limit);
    const auto recordVector = generateRecordsVector(alarmEventsRows);

    auto response = std::make_unique<db::query::alarmEvents::GetRecurringBetweenDatesResult>(recordVector);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::RemoveResult> AlarmEventRecordInterface::runQueryImplRemove(
    std::shared_ptr<db::query::alarmEvents::Remove> query)
{
    const bool ret = eventsDB->alarmEvents.removeById(query->id);
    auto response  = std::make_unique<db::query::alarmEvents::RemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarmEvents::ToggleAllResult> AlarmEventRecordInterface::runQueryImplRemove(
    std::shared_ptr<db::query::alarmEvents::ToggleAll> query)
{
    const bool ret = eventsDB->alarmEvents.toggleAll(query->toggle);
    auto response  = std::make_unique<db::query::alarmEvents::ToggleAllResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::vector<AlarmEventRecord> generateRecordsVector(const std::vector<AlarmEventsTableRow> &tableRowVector)
{
    std::vector<AlarmEventRecord> recordVector;

    recordVector.reserve(tableRowVector.size());
    for (const auto &row : tableRowVector) {
        recordVector.emplace_back(row);
    }
    return recordVector;
}

std::shared_ptr<EventRecord> AlarmEventRecord::getCopy()
{
    return std::make_shared<AlarmEventRecord>(*this);
}
