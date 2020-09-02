
#pragma once

#include "Record.hpp"
#include "Databases/SmsDB.hpp"
#include "Common/Common.hpp"

#include <utf8/UTF8.hpp>
#include <stdint.h>

struct SMSTemplateRecord : public Record
{
    UTF8 text;
    time_t lastUsageTimestamp = 0;

    SMSTemplateRecord() = default;
    SMSTemplateRecord(const SMSTemplateTableRow &);
};

enum class SMSTemplateRecordField
{
};

class SMSTemplateRecordInterface : public RecordInterface<SMSTemplateRecord, SMSTemplateRecordField>
{
  public:
    SMSTemplateRecordInterface(SmsDB *smsDb);
    ~SMSTemplateRecordInterface() = default;

    bool Add(const SMSTemplateRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(SMSTemplateRecordField field, const char *str) override final;
    bool Update(const SMSTemplateRecord &rec) override final;
    SMSTemplateRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<SMSTemplateRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<SMSTemplateRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                          uint32_t limit,
                                                                          SMSTemplateRecordField field,
                                                                          const char *str) override final;

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    SmsDB *smsDB = nullptr;
    std::unique_ptr<db::QueryResult> getByIDQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> getCountQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> addQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> removeQuery(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::QueryResult> updateQuery(std::shared_ptr<db::Query> query);
};
