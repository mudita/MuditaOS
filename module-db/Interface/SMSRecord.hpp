
/*
 * @file SMSRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Record.hpp"
#include <stdint.h>
#include "../Databases/SmsDB.hpp"
#include "../Databases/ContactsDB.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"

struct SMSRecord : public Record
{
    uint32_t date      = 0;
    uint32_t dateSent  = 0;
    uint32_t errorCode = 0;
    UTF8 number        = "";
    UTF8 body          = "";
    bool isRead        = false;
    SMSType type       = SMSType::ALL;
    uint32_t threadID  = 0;
    uint32_t contactID = 0;

    SMSRecord() = default;
    SMSRecord(const SMSTableRow &, const UTF8 & = "");
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
    ~SMSRecordInterface();

    bool Add(const SMSRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(SMSRecordField field, const char *str) override final;
    bool Update(const SMSRecord &rec) override final;
    SMSRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;
    uint32_t GetCount(EntryState state);
    uint32_t GetLastID(void);
    std::unique_ptr<std::vector<SMSRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<SMSRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                  uint32_t limit,
                                                                  SMSRecordField field,
                                                                  const char *str) override final;

  private:
    const uint32_t snippetLength = 45;
    SmsDB *smsDB;
    ContactsDB *contactsDB;
};
