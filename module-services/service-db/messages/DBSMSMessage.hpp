#pragma once

#include "DBMessage.hpp"
#include "Interface/SMSRecord.hpp"

class DBSMSMessage : public DBMessage
{
  public:
    DBSMSMessage(MessageType messageType, const SMSRecord &rec = SMSRecord{});
    SMSRecord record;
};

class DBSMSGetCount : public DBSMSMessage
{
  public:
    EntryState state;
    DBSMSGetCount(EntryState state = EntryState::ALL);
};

class DBSMSResponseMessage : public DBResponseMessage
{
  public:
    DBSMSResponseMessage(std::unique_ptr<std::vector<SMSRecord>> rec,
                         uint32_t retCode   = 0,
                         uint32_t count     = 0,
                         MessageType respTo = MessageType::MessageTypeUninitialized);
    std::unique_ptr<std::vector<SMSRecord>> records;
};
