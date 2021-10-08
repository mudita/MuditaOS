// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmsRecord.hpp"
#include "queries/alarms/QueryAlarmsAdd.hpp"
#include "queries/alarms/QueryAlarmsRemove.hpp"
#include "queries/alarms/QueryAlarmsEdit.hpp"
#include "queries/alarms/QueryAlarmsGet.hpp"
#include "queries/alarms/QueryAlarmsGetLimited.hpp"
#include "queries/alarms/QueryAlarmsTurnOffAll.hpp"

AlarmsRecord::AlarmsRecord(const AlarmsTableRow &tableRow)
    : Record{tableRow.ID}, time{tableRow.time}, snooze{tableRow.snooze}, status{tableRow.status},
      repeat{tableRow.repeat}, path{tableRow.path}
{}

AlarmsRecordInterface::AlarmsRecordInterface(AlarmsDB *alarmsDb) : alarmsDB(alarmsDb)
{}

AlarmsRecordInterface::~AlarmsRecordInterface()
{}

bool AlarmsRecordInterface::Add(const AlarmsRecord &rec)
{
    // Create alarm
    alarmsDB->alarms.add(AlarmsTableRow(rec));

    return true;
}

uint32_t AlarmsRecordInterface::GetCount()
{
    return alarmsDB->alarms.count();
}

std::unique_ptr<std::vector<AlarmsRecord>> AlarmsRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                        uint32_t limit,
                                                                                        AlarmsRecordField field,
                                                                                        const char *str)
{
    auto records = std::make_unique<std::vector<AlarmsRecord>>();

    std::vector<AlarmsTableRow> alarm;

    switch (field) {
    case AlarmsRecordField::Time:
        alarm = alarmsDB->alarms.getLimitOffsetByField(offset, limit, AlarmsTableFields::Time, str);
        break;
    case AlarmsRecordField::Snooze:
        alarm = alarmsDB->alarms.getLimitOffsetByField(offset, limit, AlarmsTableFields::Snooze, str);
        break;
    case AlarmsRecordField::Status:
        alarm = alarmsDB->alarms.getLimitOffsetByField(offset, limit, AlarmsTableFields::Status, str);
        break;
    default:
        return records;
    }
    return records;
}

std::unique_ptr<std::vector<AlarmsRecord>> AlarmsRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto alarms = alarmsDB->alarms.getLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<AlarmsRecord>>();

    for (const auto &alarm : alarms) {
        records->push_back(AlarmsRecord(alarm));
    }

    return records;
}

bool AlarmsRecordInterface::Update(const AlarmsRecord &rec)
{

    auto alarm = alarmsDB->alarms.getById(rec.ID);
    if (alarm.ID == 0) {
        return false;
    }

    alarmsDB->alarms.update(AlarmsTableRow(rec));

    return true;
}

bool AlarmsRecordInterface::RemoveByID(uint32_t id)
{

    auto alarm = alarmsDB->alarms.getById(id);
    if (alarm.ID == 0) {
        return false;
    }

    // Remove alarm
    if (alarmsDB->alarms.removeById(id) == false) {
        return false;
    }

    return true;
}

bool AlarmsRecordInterface::RemoveByField(AlarmsRecordField field, const char *str)
{

    switch (field) {
    case AlarmsRecordField::Time:
        return alarmsDB->alarms.removeByField(AlarmsTableFields::Time, str);

    default:
        return false;
    }
}

AlarmsRecord AlarmsRecordInterface::GetByID(uint32_t id)
{
    return AlarmsRecord(alarmsDB->alarms.getById(id));
}

bool AlarmsRecordInterface::TurnOffAll()
{
    return alarmsDB->alarms.updateStatuses(AlarmStatus::Off);
}

std::unique_ptr<db::QueryResult> AlarmsRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::alarms::Get)) {
        return runQueryImplGetResult(query);
    }
    if (typeid(*query) == typeid(db::query::alarms::GetLimited)) {
        return runQueryImplGetLimitedResult(query);
    }
    if (typeid(*query) == typeid(db::query::alarms::TurnOffAll)) {
        return runQueryImplTurnOffAll(query);
    }
    if (typeid(*query) == typeid(db::query::alarms::Add)) {
        return runQueryImplAdd(query);
    }
    if (typeid(*query) == typeid(db::query::alarms::Remove)) {
        return runQueryImplRemove(query);
    }
    if (typeid(*query) == typeid(db::query::alarms::Edit)) {
        return runQueryImplEdit(query);
    }
    return nullptr;
}

std::unique_ptr<db::query::alarms::GetResult> AlarmsRecordInterface::runQueryImplGetResult(
    std::shared_ptr<db::Query> query)
{
    auto getQuery = static_cast<db::query::alarms::Get *>(query.get());
    auto records  = GetByID(getQuery->id);
    auto response = std::make_unique<db::query::alarms::GetResult>(records);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarms::GetLimitedResult> AlarmsRecordInterface::runQueryImplGetLimitedResult(
    std::shared_ptr<db::Query> query)
{
    auto getLimitedQuery = static_cast<db::query::alarms::GetLimited *>(query.get());
    auto records         = GetLimitOffset(getLimitedQuery->offset, getLimitedQuery->limit);
    std::vector<AlarmsRecord> recordVector;

    for (const auto &alarm : *records) {
        recordVector.emplace_back(alarm);
    }
    auto response = std::make_unique<db::query::alarms::GetLimitedResult>(recordVector, GetCount());
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarms::AddResult> AlarmsRecordInterface::runQueryImplAdd(std::shared_ptr<db::Query> query)
{
    auto addQuery = static_cast<db::query::alarms::Add *>(query.get());
    bool ret      = Add(addQuery->getRecord());
    auto response = std::make_unique<db::query::alarms::AddResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarms::RemoveResult> AlarmsRecordInterface::runQueryImplRemove(
    std::shared_ptr<db::Query> query)
{
    auto removeQuery = static_cast<db::query::alarms::Remove *>(query.get());
    bool ret         = RemoveByID(removeQuery->id);
    auto response    = std::make_unique<db::query::alarms::RemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarms::EditResult> AlarmsRecordInterface::runQueryImplEdit(std::shared_ptr<db::Query> query)
{
    auto editQuery = static_cast<db::query::alarms::Edit *>(query.get());
    bool ret       = Update(editQuery->getRecord());
    auto response  = std::make_unique<db::query::alarms::EditResult>(ret);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::query::alarms::TurnOffAllResult> AlarmsRecordInterface::runQueryImplTurnOffAll(
    std::shared_ptr<db::Query> query)
{
    auto result   = TurnOffAll();
    auto response = std::make_unique<db::query::alarms::TurnOffAllResult>(result);
    response->setRequestQuery(query);
    return response;
}
