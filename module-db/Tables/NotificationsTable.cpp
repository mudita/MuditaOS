#include "NotificationsTable.hpp"
#include "module-db/Interface/NotificationsRecord.hpp"

#include <log/log.hpp>
#include <Utils.hpp>

#include <cassert>

NotificationsTable::NotificationsTable(Database *db) : Table(db)
{}

bool NotificationsTable::create()
{
    if (!db->Execute(createTableQuery)) {
        return false;
    }

    if (!add({{.ID = 0}, .key = static_cast<uint32_t>(NotificationsRecord::Key::Calls), .value = 0})) {
        return false;
    }
    if (!add({{.ID = 0}, .key = static_cast<uint32_t>(NotificationsRecord::Key::Sms), .value = 0})) {
        return false;
    }

    return true;
}

bool NotificationsTable::add(NotificationsTableRow entry)
{
    return db->Execute("INSERT or IGNORE INTO notifications (key, value) VALUES (%lu, %lu);", entry.key, entry.value);
}

bool NotificationsTable::removeById(uint32_t id)
{
    return db->Execute("DELETE FROM notifications where _id = %lu;", id);
}

bool NotificationsTable::removeByField(NotificationsTableFields field, const char *str)
{
    std::string fieldName;

    switch (field) {
    case NotificationsTableFields::key:
        fieldName = "key";
        break;
    }

    return db->Execute("DELETE FROM notifications where %q = '%q';", fieldName.c_str(), str);
}

bool NotificationsTable::update(NotificationsTableRow entry)
{
    return db->Execute(
        "UPDATE notifications SET key = %lu, value = %lu WHERE _id = %lu;", entry.key, entry.value, entry.ID);
}

NotificationsTableRow NotificationsTable::getById(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM notifications WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return NotificationsTableRow();
    }

    assert(retQuery->GetRowCount() == 1);

    return NotificationsTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetUInt32(), // key
        (*retQuery)[2].GetUInt32()  // value

    };
}

NotificationsTableRow NotificationsTable::GetByKey(uint32_t key)
{
    auto retQuery = db->Query("SELECT * FROM notifications WHERE key= %u;", key);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return NotificationsTableRow();
    }

    assert(retQuery->GetRowCount() == 1);

    return NotificationsTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetUInt32(), // key
        (*retQuery)[2].GetUInt32()  // value
    };
}

std::vector<NotificationsTableRow> NotificationsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->Query("SELECT * from notifications LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<NotificationsTableRow>();
    }

    std::vector<NotificationsTableRow> ret;

    do {
        ret.push_back(NotificationsTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // key
            (*retQuery)[2].GetUInt32(), // value
        });
    } while (retQuery->NextRow());

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
    auto queryRet = db->Query("SELECT COUNT(*) FROM notifications;");

    if (queryRet == nullptr || queryRet->GetRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].GetUInt32();
}

uint32_t NotificationsTable::countByFieldId(const char *field, uint32_t id)
{
    assert(0 && "Not implemented");

    return 0;
}
