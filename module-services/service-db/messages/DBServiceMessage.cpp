// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBServiceMessage.hpp>

DBServiceMessageBackup::DBServiceMessageBackup(MessageType messageType, std::string backupPath)
    : DBMessage(messageType), backupPath(backupPath)
{}

DBServiceMessageSyncPackage::DBServiceMessageSyncPackage(MessageType messageType, const std::string &syncPackagePath)
    : DBMessage(messageType), syncPackagePath(syncPackagePath)
{}

DBServiceResponseMessage::DBServiceResponseMessage(uint32_t retCode, uint32_t count, MessageType respTo)
    : DBResponseMessage(retCode, count, respTo){};
