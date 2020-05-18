#pragma once

#include "DBMessage.hpp"
#include "Interface/SMSTemplateRecord.hpp"

class DBSMSTemplateMessage : public DBMessage
{
  public:
    DBSMSTemplateMessage(MessageType messageType, const SMSTemplateRecord &rec = SMSTemplateRecord{});
    SMSTemplateRecord record;
};

class DBSMSTemplateGetCount : public DBSMSTemplateMessage
{
  public:
    DBSMSTemplateGetCount();
};

class DBSMSTemplateResponseMessage : public DBResponseMessage
{
  public:
    DBSMSTemplateResponseMessage(std::unique_ptr<std::vector<SMSTemplateRecord>> rec,
                                 uint32_t retCode,
                                 uint32_t limit     = 0,
                                 uint32_t offset    = 0,
                                 uint32_t count     = 0,
                                 MessageType respTo = MessageType::MessageTypeUninitialized);

    std::unique_ptr<std::vector<SMSTemplateRecord>> records;
    uint32_t limit  = 0;
    uint32_t offset = 0;
};
