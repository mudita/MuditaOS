#pragma once

#include "DBMessage.hpp"
#include "Interface/ContactRecord.hpp"

class DBContactMessage : public DBMessage
{
  public:
    DBContactMessage(MessageType messageType, const ContactRecord &rec = ContactRecord{}, bool favourite = false);

    ContactRecord record;
    bool favourite = false;
};

class DBContactBlock : public DBMessage
{
  public:
    DBContactBlock(MessageType messageType, const uint32_t _id, bool _shouldBeBlocked);

    uint32_t id;
    bool shouldBeBlocked;
};

class DBContactSearchMessage : public DBMessage
{
  public:
    DBContactSearchMessage(MessageType messageType,
                           const char *_primaryName,
                           const char *_alternativeName,
                           const char *_number);

    const char *primaryName;
    const char *alternativeName;
    const char *number;
};

class DBContactResponseMessage : public DBResponseMessage
{
  public:
    DBContactResponseMessage(std::unique_ptr<std::vector<ContactRecord>> rec,
                             uint32_t retCode   = 0,
                             uint32_t limit     = 0,
                             uint32_t offset    = 0,
                             bool favourite     = false,
                             uint32_t count     = 0,
                             MessageType respTo = MessageType::MessageTypeUninitialized);

    std::unique_ptr<std::vector<ContactRecord>> records;
    bool favourite  = false;
    uint32_t limit  = 0;
    uint32_t offset = 0;
};
