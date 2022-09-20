// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmEventRecord.hpp"

#include <databases/EventsDB.hpp>
#include <queries/alarm_events/QueryAlarmEventsAdd.hpp>
#include <queries/alarm_events/QueryAlarmEventsEdit.hpp>
#include <queries/alarm_events/QueryAlarmEventsGet.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetEnabled.hpp>
#include <queries/alarm_events/QueryAlarmEventsGetInRange.hpp>
#include <queries/alarm_events/QueryAlarmEventsRemove.hpp>
#include <queries/alarm_events/QueryAlarmEventsToggleAll.hpp>

#include <rrule/rrule.hpp>

std::vector<AlarmEventRecord> generateRecordsVector(const std::vector<AlarmEventsTableRow> &tableRowVector);

AlarmEventRecord::AlarmEventRecord(uint32_t id,
                                   const AlarmTime alarmTime,
                                   const std::string &musicTone,
                                   bool enabled,
                                   uint32_t snoozeDuration,
                                   const std::string &rruleText)
    : EventRecord{id}, alarmTime{alarmTime}, musicTone{musicTone}, enabled{enabled},
      snoozeDuration{snoozeDuration}, rruleText{rruleText}
{}

AlarmEventRecord::AlarmEventRecord(const AlarmEventsTableRow &aeRow)
    : EventRecord{aeRow.ID}, alarmTime{std::chrono::hours{aeRow.hourOfDay}, std::chrono::minutes{aeRow.minuteOfHour}},
      musicTone{aeRow.musicTone}, enabled{aeRow.enabled}, snoozeDuration{aeRow.snoozeDuration}, rruleText{
                                                                                                    aeRow.rruleText} {};

auto AlarmEventRecord::isValid() const -> bool
{
    return (ID != 0);
}

AlarmEventRecordInterface::AlarmEventRecordInterface(EventsDB *eventsDB) : eventsDB(eventsDB)
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
    if (typeid(*query) == typeid(db::query::alarmEvents::GetEnabled)) {
        return runQueryImplGetEnabled(std::static_pointer_cast<db::query::alarmEvents::GetEnabled>(query));
    }
    if (typeid(*query) == typeid(db::query::alarmEvents::GetInRange)) {
        return runQueryImplGetInRange(std::static_pointer_cast<db::query::alarmEvents::GetInRange>(query));
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

std::unique_ptr<db::query::alarmEvents::GetInRangeResult> AlarmEventRecordInterface::runQueryImplGetInRange(
    std::shared_ptr<db::query::alarmEvents::GetInRange> query)
{
    const auto count           = eventsDB->alarmEvents.count();
    const auto alarmEventsRows = eventsDB->alarmEvents.getLimitOffset(query->offset, query->limit);
    const auto recordVector    = generateRecordsVector(alarmEventsRows);

    auto response = std::make_unique<db::query::alarmEvents::GetInRangeResult>(recordVector, count);
    response->setRequestQuery(query);

    return response;
}

std::unique_ptr<db::query::alarmEvents::GetEnabledResult> AlarmEventRecordInterface::runQueryImplGetEnabled(
    std::shared_ptr<db::query::alarmEvents::GetEnabled> query)
{
    const auto alarmEventsRows = eventsDB->alarmEvents.getEnabled();
    const auto recordVector    = generateRecordsVector(alarmEventsRows);

    auto response = std::make_unique<db::query::alarmEvents::GetEnabledResult>(recordVector);
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

SingleEventRecord AlarmEventRecord::getNextSingleEvent(TimePoint from)
{
    auto parentEvent = getCopy();

    TimePoint alarmTimePoint = nextTimePointFromHHMM(alarmTime.hourOfDay, alarmTime.minuteOfHour, from);
    if (rruleText.empty()) {
        return SingleEventRecord(parentEvent, alarmTimePoint, TIME_POINT_MAX);
    }
    else {
        rrule::RRule recurence(rruleText);
        const TimePoint timepoint = recurence.generateNextTimePoint(alarmTimePoint, from);
        return SingleEventRecord(parentEvent, timepoint, TIME_POINT_MAX);
    }
}
