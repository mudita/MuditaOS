// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"   // for DBMessage, DBResponseMessage
#include "MessageType.hpp" // for MessageType, MessageType:...

#include <stdint.h> // for uint32_t

#include "Interface/SettingsRecord.hpp"    // for SettingsRecord
#include "Interface/SettingsRecord_v2.hpp" // for SettingsRecord_v2
#include <string>                          // for string

class DBSettingsMessage : public DBMessage
{
  public:
    DBSettingsMessage(MessageType messageType, const SettingsRecord &rec = SettingsRecord{});
    DBSettingsMessage(MessageType messageType, const SettingsRecord_v2 &rec, const std::string &path);
    SettingsRecord record;
    SettingsRecord_v2 settingsRecord;
    std::string path;
};

class DBSettingsResponseMessage : public DBResponseMessage
{
  public:
    DBSettingsResponseMessage(const SettingsRecord &rec,
                              uint32_t retCode   = 0,
                              uint32_t count     = 0,
                              MessageType respTo = MessageType::MessageTypeUninitialized);
    DBSettingsResponseMessage(const SettingsRecord_v2 &rec,
                              uint32_t retCode   = 0,
                              uint32_t count     = 0,
                              MessageType respTo = MessageType::MessageTypeUninitialized);
    SettingsRecord record;
    SettingsRecord_v2 settingsRecord;
};
