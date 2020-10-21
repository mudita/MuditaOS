// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"   // for DBMessage, DBResponseMessage
#include "MessageType.hpp" // for MessageType, MessageType::Message...

#include <stdint.h> // for uint32_t

#include "Interface/SMSRecord.hpp" // for SMSRecord
#include <memory>                  // for unique_ptr
#include <vector>                  // for vector

class DBSMSMessage : public DBMessage
{
  public:
    DBSMSMessage(MessageType messageType, const SMSRecord &rec = SMSRecord{});
    SMSRecord record;
};

class DBSMSGetCount : public DBSMSMessage
{
  public:
    DBSMSGetCount();
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
