// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBServiceMessage.hpp>
#include <service-db/DBMessage.hpp>

DBServiceMessageBackup::DBServiceMessageBackup(MessageType messageType, std::string backupPath)
    : DBMessage(messageType), backupPath(backupPath)
{}

DBServiceResponseMessage::DBServiceResponseMessage(uint32_t retCode, uint32_t count, MessageType respTo)
    : DBResponseMessage(retCode, count, respTo){};
