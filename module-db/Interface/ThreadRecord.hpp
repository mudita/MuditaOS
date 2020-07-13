#pragma once

#include "Record.hpp"
#include "module-db/Databases/SmsDB.hpp"
#include "module-db/Databases/ContactsDB.hpp"
#include "module-db/Common/Common.hpp"
#include "module-db/queries/sms/QuerySMSSearch.hpp"
#include "module-db/queries/sms/QuerySmsThreadMarkAsRead.hpp"

#include <utf8/UTF8.hpp>

#include <cstdint>
struct ThreadRecord : Record
{
    uint32_t date           = 0;
    uint32_t msgCount       = 0;
    uint32_t unreadMsgCount = 0;
    UTF8 snippet            = "";
    SMSType type            = SMSType::UNKNOWN;
    uint32_t contactID      = DB_ID_NONE;

    ThreadRecord() = default;
    ThreadRecord(const ThreadsTableRow &rec)
    {
        ID             = rec.ID;
        date           = rec.date;
        msgCount       = rec.msgCount;
        unreadMsgCount = rec.unreadMsgCount;
        snippet        = rec.snippet;
        type           = rec.type;
        contactID      = rec.contactID;
    }

    bool isUnread() const
    {
        return unreadMsgCount > 0;
    }
};

enum class ThreadRecordField
{
    ContactID,
};

class ThreadRecordInterface : public RecordInterface<ThreadRecord, ThreadRecordField>
{
  public:
    ThreadRecordInterface(SmsDB *smsDb, ContactsDB *contactsDb);
    ~ThreadRecordInterface();

    bool Add(const ThreadRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool Update(const ThreadRecord &rec) override final;
    ThreadRecord GetByID(uint32_t id) override final;
    ThreadRecord GetByContact(uint32_t contact_id);

    uint32_t GetCount() override final;
    uint32_t GetCount(EntryState state);

    std::unique_ptr<std::vector<ThreadRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<ThreadRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                     uint32_t limit,
                                                                     ThreadRecordField field,
                                                                     const char *str) override final;

    std::unique_ptr<db::QueryResult> runQuery(const db::Query *query) override;

  private:
    SmsDB *smsDB;
    ContactsDB *contactsDB;

    /// for now implementation between Interface <-> Database
    /// it would only make sense to pass Query from Inteface to multiple databases to get all data we are interested in
    /// or better split it to smaller entities... this could be done with any db high level interface -  left as it is
    std::unique_ptr<db::query::SMSSearchResult> runQueryImpl(const db::query::SMSSearch *query);
    std::unique_ptr<db::query::smsthread::MarkAsReadResult> runQueryImpl(const db::query::smsthread::MarkAsRead *query);
};
