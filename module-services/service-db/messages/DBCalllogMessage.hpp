// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"   // for DBMessage, DBResponseMessage
#include "MessageType.hpp" // for MessageType, MessageType::Mes...

#include <stdint.h> // for uint32_t

#include "Common/Common.hpp"           // for EntryState
#include "Interface/CalllogRecord.hpp" // for CalllogRecord
#include <memory>                      // for unique_ptr
#include <vector>                      // for vector

class DBCalllogMessage : public DBMessage
{
  public:
    DBCalllogMessage(MessageType messageType, const CalllogRecord &rec = CalllogRecord{});
    CalllogRecord record;
};

class DBCalllogGetCount : public DBCalllogMessage
{
  public:
    EntryState state;
    DBCalllogGetCount(EntryState state);
};

class DBCalllogResponseMessage : public DBResponseMessage
{
  public:
    DBCalllogResponseMessage(std::unique_ptr<std::vector<CalllogRecord>> rec,
                             uint32_t retCode,
                             uint32_t limit     = 0,
                             uint32_t offset    = 0,
                             uint32_t count     = 0,
                             MessageType respTo = MessageType::MessageTypeUninitialized);

    std::unique_ptr<std::vector<CalllogRecord>> records;
    uint32_t limit  = 0;
    uint32_t offset = 0;
};
