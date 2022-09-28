// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>

#include "Record.hpp"
#include "ThreadRecord.hpp"
#include "module-db/Databases/SmsDB.hpp"
#include "module-db/Databases/ContactsDB.hpp"
#include "module-db/Common/Common.hpp"

#include <utf8/UTF8.hpp>
#include <PhoneNumber.hpp>

namespace db::query
{
    class SMSSearchByType;       // Forward declaration
    class SMSSearchByTypeResult; // Forward declaration
} // namespace db::query

struct SMSRecord : public Record
{
    uint32_t date      = 0;
    uint32_t errorCode = 0;
    UTF8 body;
    SMSType type      = SMSType::UNKNOWN;
    uint32_t threadID = 0;
    utils::PhoneNumber::View number;

    SMSRecord() = default;
    SMSRecord(const SMSTableRow &w)
        : Record(w.ID), date(w.date), errorCode(w.errorCode), body(w.body), type(w.type), threadID(w.threadID)
    {}
    SMSRecord(const SMSTableRow &w, const utils::PhoneNumber::View &num)
        : Record(w.ID), date(w.date), errorCode(w.errorCode), body(w.body), type(w.type), threadID(w.threadID),
          number(num)
    {}
};

enum class SMSRecordField
{
    Number,
    ThreadID,
    ContactID
};

class SMSRecordInterface : public RecordInterface<SMSRecord, SMSRecordField>
{
  public:
    SMSRecordInterface(SmsDB *smsDb, ContactsDB *contactsDb);
    ~SMSRecordInterface() = default;

    bool Add(const SMSRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(SMSRecordField field, const char *str) override final;
    bool Update(const SMSRecord &recUpdated) override final;
    SMSRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;
    uint32_t GetLastID(void);
    std::unique_ptr<std::vector<SMSRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<SMSRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                  uint32_t limit,
                                                                  SMSRecordField field,
                                                                  const char *str) override final;

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    static const uint32_t snippetLength = 45;
    SmsDB *smsDB                        = nullptr;
    ContactsDB *contactsDB              = nullptr;

    std::vector<SMSRecord> getByText(const std::string &text,
                                     const std::optional<utils::PhoneNumber::View> &phoneNumberFilter);
    std::vector<SMSRecord> getByTextAndPhoneNumber(const std::string &text,
                                                   const utils::PhoneNumber::View &phoneNumber);

    static void UpdateThreadSummary(ThreadRecord &threadToUpdate, const SMSRecord &rec);
    std::unique_ptr<db::query::SMSSearchByTypeResult> runQueryImpl(const db::query::SMSSearchByType *query);
    std::unique_ptr<db::QueryResult> getByIDQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> getByTextQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> getCountQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> addQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> removeQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> updateQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> getQuery(const std::shared_ptr<db::Query> &query);
    auto getQueryRecords(const std::shared_ptr<db::Query> &query) -> std::vector<SMSRecord>;
    auto getQueryWithTotalCount(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>;
    auto getByThreadIDQueryRecords(const std::shared_ptr<db::Query> &query) -> std::vector<SMSRecord>;
    auto getByThreadIDQuery(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>;
    auto getByThreadIDQueryWithTotalCount(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>;
    std::unique_ptr<db::QueryResult> getForListQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> getCountByThreadIDQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> getLastByThreadIDQuery(const std::shared_ptr<db::Query> &query);

    auto getPhoneNumberBySMS(const SMSRecord &sms) -> std::optional<utils::PhoneNumber::View>;
};
