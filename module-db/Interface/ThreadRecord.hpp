// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"
#include "module-db/databases/SmsDB.hpp"
#include "module-db/databases/ContactsDB.hpp"
#include "module-db/Common/Common.hpp"
#include "module-db/queries/messages/threads/QueryThreadMarkAsRead.hpp"
#include <PhoneNumber.hpp>

#include <utf8/UTF8.hpp>

#include <cstdint>
struct ThreadRecord : Record
{
    uint32_t date           = 0;
    uint32_t msgCount       = 0;
    uint32_t unreadMsgCount = 0;
    UTF8 snippet;
    SMSType type      = SMSType::UNKNOWN;
    uint32_t numberID = DB_ID_NONE;

    ThreadRecord() = default;
    ThreadRecord(const ThreadsTableRow &rec)
        : Record(rec.ID), date(rec.date), msgCount(rec.msgCount), unreadMsgCount(rec.unreadMsgCount),
          snippet(rec.snippet), type(rec.type), numberID(rec.numberID)
    {}

    bool isUnread() const
    {
        return unreadMsgCount > 0;
    }
};

enum class ThreadRecordField
{
    ContactID,
    NumberID,
};

class ThreadRecordInterface : public RecordInterface<ThreadRecord, ThreadRecordField>
{
  public:
    ThreadRecordInterface(SmsDB *smsDb, ContactsDB *contactsDb);
    ~ThreadRecordInterface() = default;

    bool Add(const ThreadRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool Update(const ThreadRecord &rec) override final;
    ThreadRecord GetByID(uint32_t id) override final;
    ThreadRecord GetByContact(uint32_t contact_id);
    ThreadRecord GetByNumber(const utils::PhoneNumber::View &phoneNumber);

    uint32_t GetCount() override final;
    uint32_t GetCount(EntryState state);

    std::unique_ptr<std::vector<ThreadRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<ThreadRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                     uint32_t limit,
                                                                     ThreadRecordField field,
                                                                     const char *str) override final;

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    SmsDB *smsDB           = nullptr;
    ContactsDB *contactsDB = nullptr;

    /// for now implementation between Interface <-> Database
    /// it would only make sense to pass Query from Inteface to multiple databases to get all data we are interested in
    /// or better split it to smaller entities... this could be done with any db high level interface -  left as it is
    std::unique_ptr<db::QueryResult> threadSearchForListQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> markAsReadQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> threadsGetQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> threadsGetQueryWithTotalCount(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> threadsGetForListQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> threadGetByIDQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> threadGetByNumberQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> threadRemoveQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> threadsGetCount(const std::shared_ptr<db::Query> &query);

    std::vector<ThreadRecord> getThreads(const std::shared_ptr<db::Query> &query);
};
