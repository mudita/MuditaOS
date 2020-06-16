#pragma once

#include "DBMessage.hpp"

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
