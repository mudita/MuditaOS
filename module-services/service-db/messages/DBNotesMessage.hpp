#pragma once

#include "DBMessage.hpp"
#include "Interface/NotesRecord.hpp"

class DBNotesMessage : public DBMessage
{
  public:
    DBNotesMessage(MessageType messageType, const NotesRecord &rec = NotesRecord{});

    NotesRecord record;
};

class DBNotesResponseMessage : public DBResponseMessage
{
  public:
    DBNotesResponseMessage(std::unique_ptr<std::vector<NotesRecord>> rec,
                           uint32_t retCode   = 0,
                           uint32_t limit     = 0,
                           uint32_t offset    = 0,
                           uint32_t count     = 0,
                           MessageType respTo = MessageType::MessageTypeUninitialized);

    std::unique_ptr<std::vector<NotesRecord>> records;
    uint32_t limit  = 0;
    uint32_t offset = 0;
};
