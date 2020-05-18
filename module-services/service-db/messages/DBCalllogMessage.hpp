#pragma once

#include "DBMessage.hpp"
#include "Interface/CalllogRecord.hpp"

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
