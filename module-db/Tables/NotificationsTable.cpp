// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsTable.hpp"
#include "Database/Database.hpp"

#include <log/log.hpp>
#include <Utils.hpp>
#include <cassert>

NotificationsTable::NotificationsTable(Database *db) : Table(db)
{}

bool NotificationsTable::create()
{
    return true;
}

bool NotificationsTable::add(NotificationsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO notifications (key, value, contact_id) VALUES (%lu, %lu, %lu);",
                       entry.key,
                       entry.value,
                       entry.contactID);
}

bool NotificationsTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM notifications where _id = %lu;", id);
}

bool NotificationsTable::removeByField(NotificationsTableFields field, const char *str)
{
    std::string fieldName;

    switch (field) {
    case NotificationsTableFields::key:
        fieldName = "key";
        break;
    }

    return db->execute("DELETE FROM notifications where %q = '%q';", fieldName.c_str(), str);
}

bool NotificationsTable::update(NotificationsTableRow entry)
{
    return db->execute("UPDATE notifications SET key = %lu, value = %lu, contact_id = %lu WHERE _id = %lu;",
                       entry.key,
                       entry.value,
                       entry.contactID,
                       entry.ID);
}

NotificationsTableRow NotificationsTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM notifications WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return NotificationsTableRow();
    }

    assert(retQuery->getRowCount() == 1);

    return NotificationsTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // key
        (*retQuery)[2].getUInt32(), // value
        (*retQuery)[3].getUInt32()  // contactID
    };
}

NotificationsTableRow NotificationsTable::getByKey(uint32_t key)
{
    auto retQuery = db->query("SELECT * FROM notifications WHERE key= %u;", key);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return NotificationsTableRow();
    }

    assert(retQuery->getRowCount() == 1);

    return NotificationsTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // key
        (*retQuery)[2].getUInt32(), // value
        (*retQuery)[3].getUInt32()  // contactID
    };
}

std::vector<NotificationsTableRow> NotificationsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from notifications LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<NotificationsTableRow>();
    }

    std::vector<NotificationsTableRow> ret;

    do {
        ret.push_back(NotificationsTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // key
            (*retQuery)[2].getUInt32(), // value
            (*retQuery)[3].getUInt32()  // contactID
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<NotificationsTableRow> NotificationsTable::getLimitOffsetByField(uint32_t offset,
                                                                             uint32_t limit,
                                                                             NotificationsTableFields field,
                                                                             const char *str)
{
    assert(0 && "Not implemented");

    return std::vector<NotificationsTableRow>();
}

uint32_t NotificationsTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM notifications;");

    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t NotificationsTable::countByFieldId(const char *field, uint32_t id)
{
    assert(0 && "Not implemented");

    return 0;
}
