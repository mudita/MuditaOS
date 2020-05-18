#pragma once

#include "DBMessage.hpp"
#include "Interface/SettingsRecord.hpp"

class DBSettingsMessage : public DBMessage
{
  public:
    DBSettingsMessage(MessageType messageType, const SettingsRecord &rec = SettingsRecord{});
    SettingsRecord record;
};

class DBSettingsResponseMessage : public DBResponseMessage
{
  public:
    DBSettingsResponseMessage(const SettingsRecord &rec,
                              uint32_t retCode   = 0,
                              uint32_t count     = 0,
                              MessageType respTo = MessageType::MessageTypeUninitialized);
    SettingsRecord record;
};
