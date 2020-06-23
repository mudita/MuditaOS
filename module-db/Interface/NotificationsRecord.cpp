#include "NotificationsRecord.hpp"

#include <log/log.hpp>
#include <Utils.hpp>

#include <cassert>
#include <vector>

NotificationsRecord::NotificationsRecord(const NotificationsTableRow &tableRow)
    : Record{tableRow.ID}, value{tableRow.value}
{
    if (tableRow.key > static_cast<uint32_t>(Key::NotValidKey) &&
        tableRow.key < static_cast<uint32_t>(Key::NumberOfKeys)) {
        key = static_cast<Key>(tableRow.key);
        return;
    }
    LOG_ERROR("Invalid record");
    key = Key::NotValidKey;
    ID  = DB_ID_NONE;
}

bool NotificationsRecord::isValidRecord() const
{
    return isValid() && gotValidKey();
}

bool NotificationsRecord::gotValidKey() const
{
    return isValidKey(key);
}

bool NotificationsRecord::isValidKey(Key key)
{
    return key != Key::NotValidKey && key != Key::NumberOfKeys;
}

std::ostream &operator<<(std::ostream &out, const NotificationsRecord &rec)
{
    out << " <id> " << rec.ID << " <key> " << static_cast<int>(rec.key) << " <value> " << rec.value;

    return out;
}

NotificationsRecordInterface::NotificationsRecordInterface(NotificationsDB *notificationsDb)
    : notificationsDb(notificationsDb)
{}

bool NotificationsRecordInterface::Add(const NotificationsRecord &rec)
{
    assert(0 && "Not implemented");

    return false;
}

std::unique_ptr<std::vector<NotificationsRecord>> NotificationsRecordInterface::GetLimitOffsetByField(
    uint32_t offset, uint32_t limit, NotificationsRecordField field, const char *str)
{
    assert(0 && "Not implemented");

    return GetLimitOffset(offset, limit);
}

std::unique_ptr<std::vector<NotificationsRecord>> NotificationsRecordInterface::GetLimitOffset(uint32_t offset,
                                                                                               uint32_t limit)
{
    assert(0 && "Not implemented");

    return std::make_unique<std::vector<NotificationsRecord>>();
}

bool NotificationsRecordInterface::Update(const NotificationsRecord &rec)
{
    auto entry = notificationsDb->notifications.GetByID(rec.ID);
    if (!entry.isValid() || entry.key != static_cast<uint32_t>(rec.key)) {
        return false;
    }

    return notificationsDb->notifications.Update(
        NotificationsTableRow{{.ID = rec.ID}, .key = static_cast<uint32_t>(rec.key), .value = rec.value});
}

bool NotificationsRecordInterface::RemoveByID(uint32_t id)
{
    assert(0 && "Not implemented");

    return false;
}

bool NotificationsRecordInterface::RemoveByField(NotificationsRecordField field, const char *str)
{
    assert(0 && "Not implemented");

    return false;
}

NotificationsRecord NotificationsRecordInterface::GetByID(uint32_t id)
{
    return notificationsDb->notifications.GetByID(id);
}

uint32_t NotificationsRecordInterface::GetCount()
{
    return notificationsDb->notifications.GetCount();
}

NotificationsRecord NotificationsRecordInterface::GetByKey(NotificationsRecord::Key key)
{
    if (!NotificationsRecord::isValidKey(key)) {
        return NotificationsRecord();
    }

    return notificationsDb->notifications.GetByKey(static_cast<uint32_t>(key));
}

// std::unique_ptr<db::QueryResult> NotificationsRecordInterface::runQuery(const db::Query *query)
// {
//     // if (const auto local_query = dynamic_cast<const db::query::entrylog::SetAllRead *>(query)) {
//     //     return runQueryImpl(local_query);
//     // }
//     return std::unique_ptr<db::QueryResult>();
// }

// std::unique_ptr<db::query::entrylog::SetAllReadResult> NotificationsRecordInterface::runQueryImpl(
//     const db::query::entrylog::SetAllRead *query)
// {
//     auto db_result = SetAllRead();
//     return std::make_unique<db::query::entrylog::SetAllReadResult>(db_result);
// }
