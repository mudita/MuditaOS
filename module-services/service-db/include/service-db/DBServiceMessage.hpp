// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"

#include <MessageType.hpp>

#include <cstdint>
#include <string>

class DBServiceMessageBackup : public DBMessage
{
  public:
    DBServiceMessageBackup(MessageType messageType, std::string backupPath);
    std::string backupPath;
};

class DBServiceMessageSyncPackage : public DBMessage
{
  public:
    DBServiceMessageSyncPackage(MessageType messageType, const std::string &syncPackagePath);
    std::string syncPackagePath;
};

class DBServiceResponseMessage : public DBResponseMessage
{
  public:
    DBServiceResponseMessage(uint32_t retCode   = 0,
                             uint32_t count     = 0,
                             MessageType respTo = MessageType::MessageTypeUninitialized);
};
