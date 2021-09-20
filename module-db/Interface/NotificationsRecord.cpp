// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsRecord.hpp"
#include "module-db/queries/notifications/QueryNotificationsGet.hpp"
#include "module-db/queries/notifications/QueryNotificationsIncrement.hpp"
#include "module-db/queries/notifications/QueryNotificationsMultipleIncrement.hpp"
#include "module-db/queries/notifications/QueryNotificationsClear.hpp"
#include "module-db/queries/notifications/QueryNotificationsGetAll.hpp"
#include "Databases/NotificationsDB.hpp"

#include <log.hpp>
#include <Utils.hpp>

#include <cassert>
#include <vector>
#include <gsl/assert>

NotificationsRecord::NotificationsRecord(const NotificationsTableRow &tableRow, std::optional<ContactRecord> record)
    : Record{tableRow.ID}, value{tableRow.value}, contactRecord{std::move(record)}
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

bool NotificationsRecord::isValidKey(Key key)
{
    return key != Key::NotValidKey && key != Key::NumberOfKeys;
}

std::ostream &operator<<(std::ostream &out, const NotificationsRecord &rec)
{
    out << " <id> " << rec.ID << " <key> " << static_cast<int>(rec.key) << " <value> " << rec.value;
    if (rec.contactRecord.has_value()) {
        out << " <contact_id> " << rec.contactRecord.value().ID;
    }
    return out;
}

NotificationsRecordInterface::NotificationsRecordInterface(NotificationsDB *notificationsDb,
                                                           ContactRecordInterface *contactsDb)
    : notificationsDb(notificationsDb), contactsDb(contactsDb)
{
    Expects(contactsDb != nullptr);
}

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
        records->push_back(NotificationsRecord{r, getContactRecord(r.contactID)});
    }

    return records;
}

bool NotificationsRecordInterface::Update(const NotificationsRecord &rec)
{
    auto entry = notificationsDb->notifications.getById(rec.ID);
    if (!entry.isValid() || entry.key != static_cast<uint32_t>(rec.key)) {
        return false;
    }

    uint32_t contactId = rec.contactRecord.has_value() ? rec.contactRecord.value().ID : DB_ID_NONE;

    auto r      = NotificationsTableRow{Record(rec.ID)};
    r.key       = static_cast<uint32_t>(rec.key);
    r.value     = rec.value;
    r.contactID = contactId;
    return notificationsDb->notifications.update(r);
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
std::optional<ContactRecord> NotificationsRecordInterface::getContactRecord(uint32_t id) const
{
    if (id != DB_ID_NONE) {
        if (auto contactRecord = contactsDb->GetByIdWithTemporary(id); contactRecord.isValid()) {
            return std::make_optional(std::move(contactRecord));
        }
    }
    return std::nullopt;
}

NotificationsRecord NotificationsRecordInterface::GetByID(uint32_t id)
{
    auto tableRow = notificationsDb->notifications.getById(id);
    return NotificationsRecord{tableRow, getContactRecord(tableRow.contactID)};
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

    auto tableRow = notificationsDb->notifications.getByKey(static_cast<uint32_t>(key));
    return NotificationsRecord{tableRow, getContactRecord(tableRow.contactID)};
}

std::unique_ptr<db::QueryResult> NotificationsRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (const auto local_query = dynamic_cast<const db::query::notifications::Get *>(query.get())) {
        return runQueryImpl(local_query);
    }
    if (const auto local_query = dynamic_cast<const db::query::notifications::Increment *>(query.get())) {
        return runQueryImpl(local_query);
    }
    if (const auto local_query = dynamic_cast<const db::query::notifications::MultipleIncrement *>(query.get())) {
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
    auto value = GetByKey(query->getKey());
    return std::make_unique<db::query::notifications::GetResult>(std::move(value));
}

std::unique_ptr<db::query::notifications::IncrementResult> NotificationsRecordInterface::runQueryImpl(
    const db::query::notifications::Increment *query)
{
    auto ret = processIncrement(query->getKey(), query->getNumber(), 1);

    return std::make_unique<db::query::notifications::IncrementResult>(ret);
}

std::unique_ptr<db::query::notifications::MultipleIncrementResult> NotificationsRecordInterface::runQueryImpl(
    const db::query::notifications::MultipleIncrement *query)
{
    auto ret            = false;
    const auto &numbers = query->getNumbers();
    std::optional<utils::PhoneNumber::View> number;

    if (!numbers.empty()) {
        if (all_of(numbers.begin(), numbers.end(), [&](const auto &number) { return number == numbers.at(0); })) {
            number = std::make_optional(numbers.at(0));
        }

        ret = processIncrement(query->getKey(), std::move(number), numbers.size());
    }
    return std::make_unique<db::query::notifications::MultipleIncrementResult>(ret);
}

std::unique_ptr<db::query::notifications::ClearResult> NotificationsRecordInterface::runQueryImpl(
    const db::query::notifications::Clear *query)
{
    auto ret = false;
    if (auto record = GetByKey(query->getKey()); record.isValid()) {
        record.value = 0;
        record.contactRecord.reset();
        ret = Update(record);
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

bool NotificationsRecordInterface::processIncrement(NotificationsRecord::Key key,
                                                    std::optional<utils::PhoneNumber::View> &&number,
                                                    size_t size)
{
    auto ret = false;

    if (auto record = GetByKey(key); record.isValid() && number.has_value()) {
        auto &currentContactRecord = record.contactRecord;
        if (size == 1) {
            if (auto numberMatch = contactsDb->MatchByNumber(number.value()); numberMatch.has_value()) {
                if (record.value == 0) {
                    currentContactRecord = std::move(numberMatch.value().contact);
                }
                else if (currentContactRecord.has_value() &&
                         numberMatch.value().contactId != currentContactRecord.value().ID) {
                    currentContactRecord.reset();
                }
            }
            else {
                currentContactRecord.reset();
            }
        }
        else if (size > 1) {
            currentContactRecord.reset();
        }
        record.value += size;
        ret = Update(record);
    }

    return ret;
}
