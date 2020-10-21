// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"   // for DBMessage, DBResponseMessage
#include "MessageType.hpp" // for MessageType, MessageType::Mess...

#include <stdint.h> // for uint32_t

#include "Common/Common.hpp"          // for EntryState, EntryState::ALL
#include "Interface/ThreadRecord.hpp" // for ThreadRecord
#include <memory>                     // for unique_ptr
#include <vector>                     // for vector

class DBThreadMessage : public DBMessage
{
  public:
    DBThreadMessage(MessageType messageType, const ThreadRecord &rec = ThreadRecord{});
    ThreadRecord record;
};

class DBThreadGetCountMessage : public DBMessage
{
  public:
    EntryState state;
    DBThreadGetCountMessage(EntryState state = EntryState::ALL);
};

class DBThreadMessageGet : public DBMessage
{
  public:
    DBThreadMessageGet(MessageType messageType, uint32_t contactID);
    ThreadRecord record = {};
    uint32_t contactID;
};

class DBThreadResponseMessage : public DBResponseMessage
{
  public:
    DBThreadResponseMessage(std::unique_ptr<std::vector<ThreadRecord>> rec,
                            uint32_t retCode   = 0,
                            uint32_t limit     = 0,
                            uint32_t offset    = 0,
                            uint32_t count     = 0,
                            MessageType respTo = MessageType::MessageTypeUninitialized);

    std::unique_ptr<std::vector<ThreadRecord>> records;
    uint32_t limit  = 0;
    uint32_t offset = 0;
};
