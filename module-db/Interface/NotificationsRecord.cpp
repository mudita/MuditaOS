#include "NotificationsRecord.hpp"
#include "module-db/queries/notifications/QueryNotificationsGet.hpp"
#include "module-db/queries/notifications/QueryNotificationsIncrement.hpp"
#include "module-db/queries/notifications/QueryNotificationsClear.hpp"
#include "module-db/queries/notifications/QueryNotificationsGetAll.hpp"

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
    if (limit == 0) {
        limit = GetCount();
    }

    auto rows = notificationsDb->notifications.getLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<NotificationsRecord>>();

    for (auto &r : rows) {
        records->push_back(r);
    }

    return records;
}

bool NotificationsRecordInterface::Update(const NotificationsRecord &rec)
{
    auto entry = notificationsDb->notifications.getById(rec.ID);
    if (!entry.isValid() || entry.key != static_cast<uint32_t>(rec.key)) {
        return false;
    }

    return notificationsDb->notifications.update(
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
    return notificationsDb->notifications.getById(id);
}

uint32_t NotificationsRecordInterface::GetCount()
{
    return notificationsDb->notifications.count();
}

NotificationsRecord NotificationsRecordInterface::GetByKey(NotificationsRecord::Key key)
{
    if (!NotificationsRecord::isValidKey(key)) {
        return NotificationsRecord();
    }

    return notificationsDb->notifications.GetByKey(static_cast<uint32_t>(key));
}

std::unique_ptr<db::QueryResult> NotificationsRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (const auto local_query = dynamic_cast<const db::query::notifications::Get *>(query.get())) {
        return runQueryImpl(local_query);
    }
    if (const auto local_query = dynamic_cast<const db::query::notifications::Increment *>(query.get())) {
        return runQueryImpl(local_query);
    }
    if (const auto local_query = dynamic_cast<const db::query::notifications::Clear *>(query.get())) {
        return runQueryImpl(local_query);
    }
    if (const auto local_query = dynamic_cast<const db::query::notifications::GetAll *>(query.get())) {
        return runQueryImpl(local_query);
    }
    return nullptr;
}

std::unique_ptr<db::query::notifications::GetResult> NotificationsRecordInterface::runQueryImpl(
    const db::query::notifications::Get *query)
{
    auto value = GetByKey(query->key);
    return std::make_unique<db::query::notifications::GetResult>(value);
}

std::unique_ptr<db::query::notifications::IncrementResult> NotificationsRecordInterface::runQueryImpl(
    const db::query::notifications::Increment *query)
{
    auto ret = false;

    auto record = GetByKey(query->key);
    if (record.isValid() && record.key == query->key) {
        record.value++;
        ret = Update(record);
    }
    return std::make_unique<db::query::notifications::IncrementResult>(ret);
}

std::unique_ptr<db::query::notifications::ClearResult> NotificationsRecordInterface::runQueryImpl(
    const db::query::notifications::Clear *query)
{
    auto ret = false;

    auto record = GetByKey(query->key);
    if (record.isValid() && record.key == query->key) {
        record.value = 0;
        ret          = Update(record);
    }
    return std::make_unique<db::query::notifications::ClearResult>(ret);
}

std::unique_ptr<db::query::notifications::GetAllResult> NotificationsRecordInterface::runQueryImpl(
    const db::query::notifications::GetAll *query)
{
    auto numberOfNotifications = GetCount();
    auto records               = GetLimitOffset(0, numberOfNotifications);
    return std::make_unique<db::query::notifications::GetAllResult>(std::move(records));
}
