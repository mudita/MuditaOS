// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"

#include "DBMessage.hpp"                   // for DBMessage, DBResponseMessage
#include "Interface/SettingsRecord_v2.hpp" // for SettingsRecord_v2
#include "MessageType.hpp"                 // for MessageType, MessageType::MessageTypeUninitialized

class DBSettingsMessage : public DBMessage
{
  public:
    DBSettingsMessage(MessageType messageType, const SettingsRecord_v2 &rec, const std::string &path);
    SettingsRecord_v2 settingsRecord;
    std::string path;
};

class DBSettingsResponseMessage : public DBResponseMessage
{
  public:
    DBSettingsResponseMessage(const SettingsRecord_v2 &rec,
                              uint32_t retCode   = 0,
                              uint32_t count     = 0,
                              MessageType respTo = MessageType::MessageTypeUninitialized);
    SettingsRecord_v2 settingsRecord;
};
