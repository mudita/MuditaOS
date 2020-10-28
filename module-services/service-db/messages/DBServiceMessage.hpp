// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h> // for uint32_t
#include <string>   // for string

#include "DBMessage.hpp"   // for DBMessage, DBResponseMessage
#include "MessageType.hpp" // for MessageType, MessageType::MessageTypeUninitialized

class DBServiceMessageBackup : public DBMessage
{
  public:
    DBServiceMessageBackup(MessageType messageType, std::string backupPath);
    std::string backupPath;
};

class DBServiceResponseMessage : public DBResponseMessage
{
  public:
    DBServiceResponseMessage(uint32_t retCode   = 0,
                             uint32_t count     = 0,
                             MessageType respTo = MessageType::MessageTypeUninitialized);
};
