#include "NotificationsTable.hpp"
#include "module-db/Interface/NotificationsRecord.hpp"

#include <log/log.hpp>
#include <Utils.hpp>

#include <cassert>

NotificationsTable::NotificationsTable(Database *db) : Table(db)
{}

bool NotificationsTable::create()
{
    if (!db->execute(createTableQuery)) {
        return false;
    }

    if (!add({{.ID = 0}, .key = static_cast<uint32_t>(NotificationsRecord::Key::Calls), .value = 2})) {
        return false;
    }
    if (!add({{.ID = 0}, .key = static_cast<uint32_t>(NotificationsRecord::Key::Sms), .value = 3})) {
        return false;
    }
    if (!add({{.ID = 0}, .key = static_cast<uint32_t>(NotificationsRecord::Key::CalendarEvents), .value = 4})) {
        return false;
    }

    return true;
}

bool NotificationsTable::add(NotificationsTableRow entry)
{
    return db->execute("INSERT or IGNORE INTO notifications (key, value) VALUES (%lu, %lu);", entry.key, entry.value);
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
    return db->execute(
        "UPDATE notifications SET key = %lu, value = %lu WHERE _id = %lu;", entry.key, entry.value, entry.ID);
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
        (*retQuery)[2].getUInt32()  // value

    };
}

NotificationsTableRow NotificationsTable::GetByKey(uint32_t key)
{
    auto retQuery = db->query("SELECT * FROM notifications WHERE key= %u;", key);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return NotificationsTableRow();
    }

    assert(retQuery->getRowCount() == 1);

    return NotificationsTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // key
        (*retQuery)[2].getUInt32()  // value
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
