// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Common.hpp"
#include "module-db/Tables/NotificationsTable.hpp"
#include "Record.hpp"
#include "Interface/ContactRecord.hpp"

#include <utf8/UTF8.hpp>
#include <cstdint>
#include <vector>

// fw declarations
class ContactRecordInterface;
class NotificationsDB;

namespace db::query::notifications
{
    class Get;
    class GetResult;
    class Increment;
    class MultipleIncrement;
    class MultipleIncrementResult;
    class IncrementResult;
    class Clear;
    class ClearResult;
    class GetAll;
    class GetAllResult;
} // namespace db::query::notifications

struct NotificationsRecord : public Record
{
    enum class Key
    {
        NotValidKey = 0,
        Calls,
        Sms,
        NumberOfKeys // do not use directly
    };

    Key key        = Key::NotValidKey;
    uint32_t value = 0;
    std::optional<ContactRecord> contactRecord;

    friend std::ostream &operator<<(std::ostream &out, const NotificationsRecord &point);

    NotificationsRecord() = default;
    explicit NotificationsRecord(const NotificationsTableRow &tableRow,
                                 std::optional<ContactRecord> record = std::nullopt);

    static bool isValidKey(Key key);
};

enum class NotificationsRecordField
{
    key
};

class NotificationsRecordInterface : public RecordInterface<NotificationsRecord, NotificationsRecordField>
{
  public:
    NotificationsRecordInterface(NotificationsDB *notificationsDb, ContactRecordInterface *contactsDb);

    bool Add(const NotificationsRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(NotificationsRecordField field, const char *str) override final;
    bool Update(const NotificationsRecord &rec) override final;
    NotificationsRecord GetByID(uint32_t id) override final;
    uint32_t GetCount() override final;
    std::unique_ptr<std::vector<NotificationsRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::unique_ptr<std::vector<NotificationsRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                            uint32_t limit,
                                                                            NotificationsRecordField field,
                                                                            const char *str) override final;

    NotificationsRecord GetByKey(NotificationsRecord::Key key);

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    NotificationsDB *notificationsDb   = nullptr;
    ContactRecordInterface *contactsDb = nullptr;

    std::optional<ContactRecord> getContactRecord(uint32_t id) const;
    std::unique_ptr<db::query::notifications::GetResult> runQueryImpl(const db::query::notifications::Get *query);
    std::unique_ptr<db::query::notifications::IncrementResult> runQueryImpl(
        const db::query::notifications::Increment *query);
    std::unique_ptr<db::query::notifications::MultipleIncrementResult> runQueryImpl(
        const db::query::notifications::MultipleIncrement *query);
    std::unique_ptr<db::query::notifications::ClearResult> runQueryImpl(const db::query::notifications::Clear *query);
    std::unique_ptr<db::query::notifications::GetAllResult> runQueryImpl(const db::query::notifications::GetAll *query);
    [[nodiscard]] bool processIncrement(NotificationsRecord::Key key,
                                        std::optional<utils::PhoneNumber::View> &&number,
                                        size_t size);
};
