// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "SMSAlertOverridesTable.hpp"

#include "Common/Types.hpp"

namespace
{
    auto unpackRow(std::unique_ptr<QueryResult> &retQuery) -> SMSAlertOverridesTableRow
    {
        if (retQuery == nullptr || retQuery->getRowCount() == 0) {
            return SMSAlertOverridesTableRow{};
        }

        return SMSAlertOverridesTableRow{
            Record((*retQuery)[0].getUInt32()),
            .normalizedNumber = (*retQuery)[1].getString(),
            .soundPath        = (*retQuery)[2].getString(),
            .ignoreDnd        = (*retQuery)[3].getUInt32(),
            .forceVibration   = (*retQuery)[4].getUInt32(),
            .forceMaxVolume   = (*retQuery)[5].getUInt32(),
            .repeatIntervalSec = (*retQuery)[6].getUInt32(),
            .enabled          = (*retQuery)[7].getUInt32()};
    }
} // namespace

SMSAlertOverridesTable::SMSAlertOverridesTable(Database *db) : Table(db)
{}

bool SMSAlertOverridesTable::create()
{
    return true;
}

bool SMSAlertOverridesTable::add(SMSAlertOverridesTableRow entry)
{
    return db->execute("INSERT OR REPLACE INTO sms_alert_overrides "
                       "(normalized_number, sound_path, ignore_dnd, force_vibration, force_max_volume, repeat_interval_sec, enabled) "
                       "VALUES (" str_c str_c u32_c u32_c u32_c u32_c u32_ ");",
                       entry.normalizedNumber.c_str(),
                       entry.soundPath.c_str(),
                       entry.ignoreDnd,
                       entry.forceVibration,
                       entry.forceMaxVolume,
                       entry.repeatIntervalSec,
                       entry.enabled);
}

bool SMSAlertOverridesTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM sms_alert_overrides WHERE _id=" u32_ ";", id);
}

bool SMSAlertOverridesTable::update(SMSAlertOverridesTableRow entry)
{
    return db->execute(
    "UPDATE sms_alert_overrides SET normalized_number=" str_c "sound_path=" str_c "ignore_dnd=" u32_c
    "force_vibration=" u32_c "force_max_volume=" u32_c "repeat_interval_sec=" u32_c
    "enabled=" u32_ " WHERE _id=" u32_ ";",
        entry.normalizedNumber.c_str(),
        entry.soundPath.c_str(),
        entry.ignoreDnd,
    entry.forceVibration,
    entry.forceMaxVolume,
    entry.repeatIntervalSec,
        entry.enabled,
        entry.ID);
}

SMSAlertOverridesTableRow SMSAlertOverridesTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM sms_alert_overrides WHERE _id=" u32_ ";", id);
    return unpackRow(retQuery);
}

SMSAlertOverridesTableRow SMSAlertOverridesTable::getByNumber(const std::string &normalizedNumber)
{
    auto retQuery =
        db->query("SELECT * FROM sms_alert_overrides WHERE normalized_number=" str_ " LIMIT 1;",
                  normalizedNumber.c_str());
    return unpackRow(retQuery);
}

std::vector<SMSAlertOverridesTableRow> SMSAlertOverridesTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * FROM sms_alert_overrides ORDER BY _id LIMIT " u32_ " OFFSET " u32_ ";",
                              limit,
                              offset);

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return {};
    }

    std::vector<SMSAlertOverridesTableRow> result;
    do {
        result.push_back(unpackRow(retQuery));
    } while (retQuery->nextRow());

    return result;
}

std::vector<SMSAlertOverridesTableRow> SMSAlertOverridesTable::getLimitOffsetByField(uint32_t offset,
                                                                                      uint32_t limit,
                                                                                      SMSAlertOverridesTableFields field,
                                                                                      const char *str)
{
    if (field != SMSAlertOverridesTableFields::normalizedNumber) {
        return {};
    }

    auto retQuery = db->query("SELECT * FROM sms_alert_overrides WHERE normalized_number=" str_ " ORDER BY _id LIMIT "
                              u32_ " OFFSET " u32_ ";",
                              str,
                              limit,
                              offset);

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return {};
    }

    std::vector<SMSAlertOverridesTableRow> result;
    do {
        result.push_back(unpackRow(retQuery));
    } while (retQuery->nextRow());

    return result;
}

uint32_t SMSAlertOverridesTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM sms_alert_overrides;");
    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }
    return (*queryRet)[0].getUInt32();
}

uint32_t SMSAlertOverridesTable::countByFieldId(const char *field, uint32_t id)
{
    (void)field;
    (void)id;
    return 0;
}