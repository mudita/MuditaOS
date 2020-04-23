
#pragma once

#include "Record.hpp"
#include "Databases/SmsDB.hpp"
#include "Common/Common.hpp"

#include <utf8/UTF8.hpp>
#include <stdint.h>

struct SMSTemplatesRecord : public Record
{
    UTF8 text;
    uint32_t lastUsageTimestamp = 0;

    SMSTemplatesRecord() = default;
    SMSTemplatesRecord(const SMSTemplatesTableRow &);
};

enum class SMSTemplatesRecordField
{
};

class SMSTemplatesRecordInterface : public RecordInterface<SMSTemplatesRecord, SMSTemplatesRecordField>
{
  public:
    SMSTemplatesRecordInterface(SmsDB *smsDb);
    ~SMSTemplatesRecordInterface();

    bool Add(const SMSTemplatesRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(SMSTemplatesRecordField field, const char *str) override final;
    bool Update(const SMSTemplatesRecord &rec) override final;
    SMSTemplatesRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<SMSTemplatesRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<SMSTemplatesRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                           uint32_t limit,
                                                                           SMSTemplatesRecordField field,
                                                                           const char *str) override final;

  private:
    SmsDB *smsDB;
};
