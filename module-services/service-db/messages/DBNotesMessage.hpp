// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"   // for DBMessage, DBResponseMessage
#include "MessageType.hpp" // for MessageType, MessageType::Messa...

#include <stdint.h> // for uint32_t

#include "Interface/NotesRecord.hpp" // for NotesRecord
#include <memory>                    // for unique_ptr
#include <vector>                    // for vector

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
