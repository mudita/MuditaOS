/*
 * AlarmsRecord.cpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#include "AlarmsRecord.hpp"
#include "ThreadRecord.hpp"
#include "AlarmsTable.hpp"

AlarmsRecordInterface::AlarmsRecordInterface(AlarmsDB *alarmsDb) : alarmsDB(alarmsDb)
{}

AlarmsRecordInterface::~AlarmsRecordInterface()
{}

bool AlarmsRecordInterface::Add(const AlarmsRecord &rec)
{

    uint32_t contactID = 0;

    // Create alarm
    alarmsDB->alarms.Add(
        AlarmsTableRow{.time = rec.time, .snooze = rec.snooze, .status = rec.status, .path = rec.path});

    // TODO: error check

    return true;
}

uint32_t AlarmsRecordInterface::GetCount()
{
    return alarmsDB->alarms.GetCount();
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
        alarm = alarmsDB->alarms.GetLimitOffsetByField(offset, limit, AlarmsTableFields::Time, str);
        break;
    case AlarmsRecordField::Snooze:
        alarm = alarmsDB->alarms.GetLimitOffsetByField(offset, limit, AlarmsTableFields::Snooze, str);
        break;
    case AlarmsRecordField::Status:
        alarm = alarmsDB->alarms.GetLimitOffsetByField(offset, limit, AlarmsTableFields::Status, str);
        break;
    default:
        return records;
    }
    return records;
}

std::unique_ptr<std::vector<AlarmsRecord>> AlarmsRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto alarm = alarmsDB->alarms.GetLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<AlarmsRecord>>();

    AlarmsRecordInterface alarmsInterface(alarmsDB);
    for (const auto &w : alarm) {

        auto alarmsRec = alarmsInterface.GetByID(w.ID);

        records->push_back({
            .ID     = w.ID,
            .time   = w.time,
            .snooze = w.snooze,
            .status = w.status,
            .path   = w.path,
        });
    }

    return records;
}

bool AlarmsRecordInterface::Update(const AlarmsRecord &rec)
{

    auto alarm = alarmsDB->alarms.GetByID(rec.ID);
    if (alarm.ID == 0) {
        return false;
    }

    alarmsDB->alarms.Update(
        AlarmsTableRow{.ID = rec.ID, .time = rec.time, .snooze = rec.snooze, .status = rec.status, .path = rec.path});

    return true;
}

bool AlarmsRecordInterface::RemoveByID(uint32_t id)
{

    auto alarm = alarmsDB->alarms.GetByID(id);
    if (alarm.ID == 0) {
        return false;
    }

    // Remove alarm
    if (alarmsDB->alarms.RemoveByID(id) == false) {
        return false;
    }

    return true;
}

bool AlarmsRecordInterface::RemoveByField(AlarmsRecordField field, const char *str)
{

    switch (field) {
    case AlarmsRecordField ::Time:
        return alarmsDB->alarms.RemoveByField(AlarmsTableFields::Time, str);

    default:
        return false;
    }
}

AlarmsRecord AlarmsRecordInterface::GetByID(uint32_t id)
{
    auto alarm = alarmsDB->alarms.GetByID(id);

    return AlarmsRecord{
        .ID = alarm.ID, .time = alarm.time, .snooze = alarm.snooze, .status = alarm.status, .path = alarm.path};
}

AlarmsRecord AlarmsRecordInterface::GetNext(time_t time)
{
    auto alarm = alarmsDB->alarms.GetNext(time);

    return AlarmsRecord{
        .ID = alarm.ID, .time = alarm.time, .snooze = alarm.snooze, .status = alarm.status, .path = alarm.path};
}
