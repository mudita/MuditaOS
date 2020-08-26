
#pragma once

#include <stdint.h>

#include "Record.hpp"
#include "ThreadRecord.hpp"
#include "module-db/Databases/SmsDB.hpp"
#include "module-db/Databases/ContactsDB.hpp"
#include "module-db/queries/sms/QuerySMSSearchByType.hpp"
#include "module-db/Common/Common.hpp"

#include <utf8/UTF8.hpp>
#include <PhoneNumber.hpp>

struct SMSRecord : public Record
{
    uint32_t date      = 0;
    uint32_t dateSent  = 0;
    uint32_t errorCode = 0;
    UTF8 body;
    SMSType type       = SMSType::UNKNOWN;
    uint32_t threadID  = 0;
    uint32_t contactID = 0;
    utils::PhoneNumber::View number;

    SMSRecord() = default;
    SMSRecord(const SMSTableRow &, const utils::PhoneNumber::View &);
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

    static void UpdateThreadSummary(ThreadRecord &threadToUpdate, const SMSRecord &rec);
    std::unique_ptr<db::query::SMSSearchByTypeResult> runQueryImpl(const db::query::SMSSearchByType *query);
    std::unique_ptr<db::QueryResult> getByIDQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getByContactIDQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getByTextQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getCountQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> removeQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getQuery(std::shared_ptr<db::Query> query);
};
